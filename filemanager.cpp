#include <bits/stdc++.h>
#include <stdio.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

#define clr printf("\033[H\033[J")
#define gotoXY printf("%c[%d;%dH", 27, r, c)




using namespace std;

char  root[4096], c_dir[4096];
stack <string> bs,fs;
vector<string> dtable;
struct winsize win; 
int win_ptr=0;
int r=1,c=1;
int trow,tcol;
vector<string> tokens;
string command;


int display_dir(const string path);
void display(string s);
int traversal();
void up();
void down();
void left();
void right();
void Home();
void Enter();
void Backspace();
void parsing(string str);
int command_mode();
void Move();
void Rename();
void Copy();
void Search();
int search_R(string x,string y,int &a);
void Goto();
void create_file();
void create_dir();
void delete_file();
void delete_dir();
int delete_R(string);



int main()
{
    
    clr;
    struct termios original_tset, new_tset; 

    tcgetattr(STDIN_FILENO, &original_tset);


    new_tset=original_tset;


    
       getcwd(root, sizeof(root)); //set root as current working dir.
    
    strcat(c_dir, root);
    //bs.push(string(root)); 
    //cout<<c_dir;

   display_dir(root); 

    new_tset.c_lflag &= ~ICANON;  
    new_tset.c_lflag &= ~ECHO; 
     if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_tset) != 0) 
    {
        fprintf(stderr,"Could not set attributes\n");
    }
   else 
    {
        traversal(); 
    }

   
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tset);

                r = 1;
                c = 1;
                gotoXY;

    return 0;
}

int display_dir(const string path){

ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    trow = win.ws_row - 2;
    tcol = win.ws_col;
    clr;
    struct dirent* d;
    DIR* dir;
    dir = opendir(path.c_str());
    if (dir == NULL) {
        perror("opendir");
        return -1;
    }
    dtable.clear();
    while ((d = readdir(dir))) {
        
        dtable.push_back(d->d_name);
    }
    sort(dtable.begin(), dtable.end());
    
    clr;//to clear screen
    c = 1;
    gotoXY;
    int lsize = dtable.size();
    int start = win_ptr;
    int end;
    if ((unsigned)lsize <= trow)
        end = lsize - 1;
    else
        end = trow + win_ptr;
    for (int i = start; i <= end; i++) {
        string t = dtable[i];
        display(t);
    }

    r = 1;
    //cy = 1;
    gotoXY;
    //reset_gotoXY_top();
    closedir(dir);
    return 0;

}



void display(string s)
{
    c = 0;
    const char* dName=s.c_str();
    struct stat sb;
    //char* abs_path;
    //realpath(dName,abs_path);
    string abs_path = string(c_dir)+"/"+s;
    stat(abs_path.c_str(), &sb);
   
    if(S_ISBLK(sb.st_mode))
        printf("b");
       
    else if(S_ISCHR(sb.st_mode))
        printf("c");
        
    else if(S_ISDIR(sb.st_mode))
        printf("d");
       
    else if(S_ISFIFO(sb.st_mode))
        printf("p");
        
    else if(S_ISLNK(sb.st_mode))
        printf("l");
        
    else if(S_ISREG(sb.st_mode))
        printf("-");
        
    else if(S_ISSOCK(sb.st_mode))
        printf("s");
        
    else
        printf("-");
        

    c++;
    printf((sb.st_mode & S_IRUSR) ? "r" : "-");
    c++;
    printf((sb.st_mode & S_IWUSR) ? "w" : "-");
    c++;
    printf((sb.st_mode & S_IXUSR) ? "x" : "-");
    c++;
    printf((sb.st_mode & S_IRGRP) ? "r" : "-");
    c++;
    printf((sb.st_mode & S_IWGRP) ? "w" : "-");
    c++;
    printf((sb.st_mode & S_IXGRP) ? "x" : "-");
    c++;
    printf((sb.st_mode & S_IROTH) ? "r" : "-");
    c++;
    printf((sb.st_mode & S_IWOTH) ? "w" : "-");
    c++;
    printf((sb.st_mode & S_IXOTH) ? "x" : "-");
    c++;

    struct passwd* get_username;
    get_username = getpwuid(sb.st_uid);
    string uname = get_username->pw_name;
    c += printf(" %10s ", uname.c_str());

    struct group* get_grpname;
    get_grpname = getgrgid(sb.st_gid);
    string gname = get_grpname->gr_name;
    c+= printf(" %10s ", gname.c_str());

    long long x = sb.st_size;
    if (x >= (1 << 30))
        c+= printf("%4lldG ", x / (1 << 30));
    else if (x >= (1 << 20))
        c+= printf("%4lldM ", x / (1 << 20));
    else if (x >= (1 << 10))
        c += printf("%4lldK ", x / (1 << 10));
    else
        c += printf("%4lldB ", x);

    string m_time = string(ctime(&sb.st_mtime));
    m_time = m_time.substr(4, 12);
    c+= printf(" %-12s ", m_time.c_str());

    
        if(S_ISDIR(sb.st_mode))
    {
        printf("\033[1;33m");
        printf("\t%-20s\n", dName);
        printf("\033[0m");
    }
    else
        printf("\t%-20s\n", dName);
    c++;
}

int traversal()
{

char ch;
        while (ch=cin.get()) {
            unsigned int tp = win.ws_row;
            //cy = 1;
         
            gotoXY;
            fflush(0);
            if (ch == 27){

              ch=cin.get();
              ch=cin.get();
            
            switch (ch)
            {
                case 'A':
                up();
                break;

                case 'B':
                down();
                break;

                case 'C':
                right();
                break;

                case 'D':
                left();
                break;

                default:
                break;
            }
            
        }
        
            else if (ch == 'K' || ch == 'k')
                up();
            else if (ch == 'L' || ch == 'l')
                down();
            else if (ch == 'H' || ch == 'h')
                Home();
            else if (ch == '\n')
                Enter();
            
            else if (ch == 127)
                Backspace();
            
            else if (ch == ':') {
                int ret = command_mode();
                r = 1;
                gotoXY;
                display_dir(string(c_dir));
                /*
                if (ret == 2) {
                    search_flag = true;
                    continue;
                }
                else
                    display_dir(string(c_dir));
                    */
            }
            
            else if (ch == 'q' || ch == 'Q') {
                clr;
                
                return 0;
            }

            else{

            }
            fflush(0);
            
        }


return 0;
}

void up(){

    
    if (r == 1 && win_ptr > 0) {
        win_ptr--;
        //listdir(cur_dir);
        clr;//to clear screen
    c = 1;
    gotoXY;
    int lsize = dtable.size();
    int start = win_ptr;
    int end;
    if ((unsigned)lsize <= trow)
        end = lsize - 1;
    else
        end = trow + win_ptr;
    for (int i = start; i <= end; i++) {
        string t = dtable[i];
        display(t);
    }

        gotoXY;
    }
    else if (r > 1) {
        r--;
        gotoXY;
    }

}

void down(){
if (r <= trow && r < dtable.size()) {
        r++;
        gotoXY;
    }
    else if (r > trow && r + win_ptr < dtable.size()) {
        win_ptr++;
        //listdir(cur_dir);
        clr;//to clear screen
    c = 1;
    gotoXY;
    int lsize = dtable.size();
    int start = win_ptr;
    int end;
    if ((unsigned)lsize <= trow)
        end = lsize - 1;
    else
        end = trow + win_ptr;
    for (int i = start; i <=end; i++) {
        string t = dtable[i];
        display(t);
    }

        gotoXY;
    }
}

void left(){

    if (bs.size() > 0) {
        r=1;
win_ptr=0;
gotoXY;
        string tmp = bs.top();
        fs.push(c_dir);
        bs.pop();
        
        strcpy(c_dir, tmp.c_str());
        
         display_dir(string(c_dir));
    }
    
}

void right(){

if (fs.size()>0) {

        r=1;
win_ptr=0;
gotoXY;
        bs.push(c_dir);
        string tmp = fs.top();
        fs.pop();
        strcpy(c_dir, tmp.c_str());
       
        display_dir(string(c_dir));
    }

}

void Home(){
r=1;
win_ptr=0;
gotoXY;
    

    if( strcmp(c_dir,root) != 0 )
        {
            bs.push(c_dir);
            strcpy(c_dir, root);
        }
   
    while (fs.size()>0)
        fs.pop();
    display_dir(c_dir);

}

void Enter(){

    string path;
    path.clear();
    path=string(c_dir) + "/"+ dtable[r + win_ptr - 1] ;
    struct stat sb;
    stat(path.c_str(), &sb);

    if(S_ISDIR(sb.st_mode))
{
if (dtable[r + win_ptr - 1] == ".") {
        // if selected directory is '.' .
       
    }
    else if (dtable[r + win_ptr - 1] == ".." && strcmp(c_dir,root)!=0) {
        // if selected directory is '..' .
        bs.push(c_dir);
        string s= c_dir;


        size_t pos;
    pos = s.find_last_of("/\\");
    s=s.substr(0, pos);

    
        strcpy(c_dir, s.c_str());
        
        while (fs.size()>0)
            fs.pop();
        r=1;
        win_ptr=0;
        gotoXY;
        display_dir(c_dir);
    }
    else {
        
        bs.push(c_dir);
        strcpy(c_dir, path.c_str());
        //clear forward stack if entering in any directory manually.
        while (fs.size()>0)
            fs.pop();
        
       
             r=1;
        win_ptr=0;
        gotoXY;
        display_dir(c_dir);
        }
    }
        else {
           
            pid_t pid = fork();
            if (pid == 0) {
                close(2);
                execlp("xdg-open", "xdg-open", path.c_str(), NULL);
                exit(0);
            }
        }
    }



void Backspace(){

if (strcmp(c_dir, root) == 0)
        {

        }
    
    else {

         bs.push(c_dir);
        string s= c_dir;


        size_t pos;
    pos = s.find_last_of("/\\");
    s=s.substr(0, pos);

    
        strcpy(c_dir, s.c_str());
        
        while (fs.size()>0)
            fs.pop();
        r=1;
        win_ptr=0;
        gotoXY;
        display_dir(c_dir);
    }
    
}

int command_mode()
{
    char ch;
    while (1) {
        
        r = win.ws_row;
        c = 1;
        gotoXY;
        printf("\x1b[2K");
        printf(":");
        fflush(0);
        c++;
        
        command.clear();
        //my_command.clear();
        while (1) {
            

            ch=cin.get();
            if (ch == 27 ) {
                
                printf("\x1b[2K");
                r = 1;
                        c = 1;
                        gotoXY;
                        return 0;
                    
            }
            
            else if (ch == '\n') {
                
                command.push_back(ch);
                parsing(command);
               
                 string s = tokens[0];
                 

                    if (s == "copy")
                        {Copy();
                            break;
                        }
                    else if (s == "move"){
                        Move();
                        break;
                    }
                    else if (s == "rename"){
                        Rename();
                        break;
                    }
                   
                    else if (s == "create_file")
                        create_file();
                    else if (s == "create_dir")
                        create_dir();
                    
                    else if (s == "delete_file")
                        delete_file();
                    else if (s == "delete_dir")
                        delete_dir();
                    
                    else if (s == "goto")
                        {

                            string dest=tokens[1];
                            bs.push(c_dir);
                            strcpy(c_dir,dest.c_str());

                            r = win.ws_row;
                            c = 1;
                            gotoXY;
                            printf("\x1b[2K");
                            return 0;



                        }
                    
                    else if (s == "search")
                        Search();
                    
                    else 
                    {
                        r = win.ws_row;
                        c = 1;
                        gotoXY;
                        printf("\x1b[0K");
                        printf(":");
                        cout << "command not found." << endl;
                    }


                
                break;
                
            }
            else if (ch == 127) {
                int lastLine = win.ws_row ;
                printf("%c[%d;%dH",27,lastLine,1);
                printf("\x1b[2K");
                printf(":");
                fflush(0);
                if(command.length()==0){

                }
               else if(command.length()==1)
                {
                    command="";
                    cout<<command;
                    c--;
                    gotoXY;

                }
                else{
                    command = command.substr(0,command.length()-1);
                    cout<<command;
                    c--;
                    gotoXY;

                }

            }
            else {
                cout << ch;
                c++;
                gotoXY;
                command.push_back(ch);
            }
            fflush(0);
            
        }
    }
    return 0;
}

void parsing(string str)
{
    
    unsigned int i=0;
    tokens.clear();
    
    while(i<str.length())
    {
        string sub="";
        while(str[i]!=' ' && str[i]!='\n' && i<str.length())
        {
            if(str[i]=='\\')
            {
                sub += str[i+1];
                i=i+2;
            }
            else{

                sub += str[i++];
                
            }
            
        }
        
            tokens.push_back(sub);
            
            
        
        i++;    
    }

}

void Copy(){

 if (tokens.size() < 3)
        printf("too few arguments\n");
    else {
        string dest =tokens[tokens.size()-1];
        
        for (unsigned int i = 1; i < tokens.size() - 1; i++) {
            string from=  tokens[i];
            size_t pos = from.find_last_of("/\\");


            string to= dest + "/" + from.substr(pos + 1, from.length() - pos);
            
            
                FILE *src, *dst;
    char ch;
    if ((src = fopen(from.c_str(), "r")) == NULL) {
       
        return;
    }
    if ((dst = fopen(to.c_str(), "w")) == NULL) {
       
        return;
    }
     ch = getc(src);
    while (ch!=EOF) {
         putc(ch, dst);
        ch = getc(src);
       
    }
    struct stat from_stat;
    stat(from.c_str(), &from_stat);
    chown(to.c_str(), from_stat.st_uid, from_stat.st_gid);
    chmod(to.c_str(), from_stat.st_mode);
    fclose(src);
    fclose(dst);
            
        }
    }
   
return;

}

void Move(){

 if (tokens.size() < 3)
        printf("too few arguments\n");
    else {
        Copy();
        
        
        for (unsigned int i = 1; i < tokens.size() - 1; i++) {
            string from=  tokens[i];
            if (remove(from.c_str()) == -1)
                    perror("");
            
        }
    }
   
return;


}

void Rename(){

  
                    
                   
                        string sr,dest;
                        sr=string(c_dir)+"/" + tokens[1];
                        dest=string(c_dir)+"/" + tokens[2];
                        FILE *src, *dst;
    char ch;
    if ((src = fopen(sr.c_str(), "r")) == NULL) {
        perror("");
        return;
    }
    if ((dst = fopen(dest.c_str(), "w")) == NULL) {
        perror("");
        return;
    }
     ch = getc(src);
    while (ch!=EOF) {
         putc(ch, dst);
        ch = getc(src);
       
    }
    struct stat from_stat;
    stat(sr.c_str(), &from_stat);
    chown(dest.c_str(), from_stat.st_uid, from_stat.st_gid);
    chmod(dest.c_str(), from_stat.st_mode);
    fclose(src);
    fclose(dst);

            if (remove(sr.c_str()) == -1)
                    perror("");

                return;
}

void create_file(){

    FILE *create;
    string dest= tokens[2] + "/" + tokens[1];
        create = fopen(dest.c_str(), "w+");
            if (create == NULL)
                perror("");
            else
                cout << " File created" << endl;
            fclose(create);

}
void create_dir(){
    string dest= tokens[2] + "/" + tokens[1];
    if (mkdir(dest.c_str(), 0755) != 0)
        perror("");
    else
        cout << " Directory created" << endl;
}

void delete_file(){
string rem_path= string(root) + "/" + tokens[1];
    if (remove(rem_path.c_str()) != 0)
            perror("");
    else
            cout << " Deleted -" << tokens[1] << endl;
}

void delete_dir(){

    string rem_path= string(root) + "/" + tokens[1];
    delete_R(rem_path);
}

int delete_R(string pth){
    DIR* dp;
    dp = opendir(pth.c_str());
    struct dirent* d;
    struct stat sb;
    if (dp == NULL) {
       
        return 0;
    }
    while ((d = readdir(dp))) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;
        else {
            string tmp;
            tmp.clear();

            tmp = pth + "/" + string(d->d_name);
            
            stat(tmp.c_str(), &sb);
   
            if(S_ISDIR(sb.st_mode))
                {
                    
                  delete_R(tmp);
                }
                else{
                    remove(tmp.c_str());
                }
            
        }
    }
    rmdir(pth.c_str());
    closedir(dp);
    return 0;

}

void Search(){
int g=0;
search_R(string(c_dir),tokens[1],g);
if(g==1)
    cout<<" TRUE\n";
else
    cout<<" FALSE\n";

}
int search_R(string pth, string name,int &f){

    DIR* dp;
    dp = opendir(pth.c_str());
    struct dirent* d;
    struct stat sb;
    if (dp == NULL) {
        
        return 0;
    }
    while ((d = readdir(dp))) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
            continue;
        else {
            string tmp;
            tmp.clear();

            tmp = pth + "/" + string(d->d_name);
            if (strcmp(d->d_name, name.c_str()) == 0)
                {
                    closedir(dp);
                    f=1;
                    return 1;
                }

            stat(tmp.c_str(), &sb);
   
            if(S_ISDIR(sb.st_mode))
                {
                    
                  search_R(tmp, name,f);
                }
            
        }
    }
    closedir(dp);
    return 0;
}
