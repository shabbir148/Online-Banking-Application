#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <pthread.h>

#define SEND_BUFFER 1024
#define REC_BUFFER 1024

void send_to_client(int clientfd, char *message)
{
    int size_header = (strlen(message) - 1) / SEND_BUFFER + 1;
    int send_status = write(clientfd, &size_header, sizeof(int));

    char *send_message = (char *)malloc(size_header * SEND_BUFFER);
    strcpy(send_message, message);

    for (int i = 0; i < size_header; i++)
    {
       int send_status = write(clientfd, send_message, SEND_BUFFER);
        send_message += SEND_BUFFER;
    }
}

char *receive_from_client(int clientfd)
{
    int size_header = 0;
    int receive_status = read(clientfd, &size_header, sizeof(int));
   if (receive_status <= 0)
    {
        shutdown(clientfd, SHUT_WR);
        return NULL;
    }

    char *message = (char *)malloc(size_header * REC_BUFFER);
    memset(message, 0, size_header * REC_BUFFER);

    char *receive_message = message;

    for (int i = 0; i < size_header; i++)
    {
        int receive_status = read(clientfd, message, REC_BUFFER);
        message += REC_BUFFER;
    }
    return receive_message;
}

//void send_to_client(int clientfd, char *message)
//{

//    char *send_message = (char *)malloc(SEND_BUFFER);
//   strcpy(send_message, message); 
//    int send_status = write(clientfd, send_message, SEND_BUFFER);
       
//}


void close_connection(int clientfd, char *closing_message)
{
    send_to_client(clientfd, closing_message);
    shutdown(clientfd, SHUT_RDWR);
    close(clientfd);
}


//char *receive_from_client(int clientfd)
//{

//    char *message = (char *)malloc(REC_BUFFER);
//    memset(message, 0,REC_BUFFER);
//    char *receive_message = message;
//    int receive_status = read(clientfd, message, REC_BUFFER);      
//    return receive_message;

//}

pthread_mutex_t mx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mx3 = PTHREAD_MUTEX_INITIALIZER;





//SIGINT
void handler()
{
   printf("\n\n\nShut Down.\n......GOOOD BYE......\n");
   exit(1);
}






int deposit_withdraw_handler(int account_num, int operation, int amount,int type)
{

    int fd2;
    int fd3;
    char* buffer = (char *)malloc(240);

    int retval = 0;

    pthread_mutex_lock(&mx1);
    
    fd2 = open("Persnol_Information.txt", O_RDONLY, 0);
    
    if (fd2 < 0)
    {
        perror("\n>>>---------------------------------------<<<\nCould not open the file containing accounts information.\n");
        exit(1);
    }

    fd3 = open("Persnol_InformationDuplicate.txt", O_WRONLY | O_CREAT, 0777);
    
    if (fd3 < 0)
    {
        perror("\n>>>---------------------------------------<<<\nCould not create duplicate file containing accounts information.\n");
        exit(1);
    }
    
    int type1=type;
    int acc_num = account_num;
    int op = operation;
    int amt = amount;
    int fd8=open("Transection_detail.txt",O_WRONLY | O_CREAT,0777);
    int i = 0;
    while (read(fd2, &buffer[i], 1) == 1)
    {
        if (buffer[i] == '\n' || buffer[i] == 0x0)
        {
            buffer[i] = 0;
            char buffer_1[240];
            char *token = strtok(buffer, " "); //Account ID
            strcpy(buffer_1, token);
            strcat(buffer_1, " ");
            int read_accid = atoi(token);
            token = strtok(NULL, " "); //First Name
            strcat(buffer_1, token); 
            strcat(buffer_1, " ");
            token = strtok(NULL, " "); //Last Name
            strcat(buffer_1, token); 
            strcat(buffer_1, " ");
            token = strtok(NULL, " "); //Account Type
            char *token1 =token;
            strcat(buffer_1, token); 
            strcat(buffer_1, " ");  
            token = strtok(NULL, " "); //Amount
           

            if(read_accid == acc_num)
            {
                char *str=(char *)malloc(240);
                int read_amt = atoi(token);
                int new_amt;
                
                if (op == 1)
                {
                    new_amt = read_amt + amt;
                    sprintf(str,"Deposite : +%d",amt);
                }
                else
                {
                    if((read_amt - amt) >= 0)
                    {
                        new_amt = read_amt - amt;
                        sprintf(str,"withrawl : +%d",amt);
                    }
                    else
                    {
                        retval = -1;
                        new_amt = read_amt;
                        sprintf(str,"no money : +%d",amt);
                   
                    }
                    
                }
                
                char buf[140];
                time_t mytime = time(NULL);
                char * time_str = ctime(&mytime);
                time_str[strlen(time_str)-1] = '\0'; 
                strcpy(buf, time_str);
                strcat(buf, " ");
                strcat(buf, token1);
                strcat(buf, " ");
                sprintf(str,"Deposite : +%d",amt);
                strcat(buf, str);
                strcat(buf,"\n");
                lseek(fd8,0L,SEEK_END);
                
                int w1=write(fd8,buf,sizeof(buf));  
                
                
                char* new_amt_str = (char *)malloc(240);
                
                sprintf(new_amt_str, "%d", new_amt);
                
                strncpy(token, new_amt_str, strlen(new_amt_str)); 
                int n = strlen(token) - strlen(new_amt_str);
           
                if(n > 0)
                {
                    for (int i = 0; i < n; i++)
                    {
                        token[strlen(new_amt_str) + i] = '\0';
                    }
                }
            }
            strcat(buffer_1, token); 
            strcat(buffer_1, "\n"); 

            int w = write(fd3, buffer_1, strlen(buffer_1));

            i = 0;
            continue;
        }

        i += 1;
    }
    
    close(fd2);
    close(fd3);
    
    int rem = remove("Persnol_Information.txt");
   
    int ren = rename("Persnol_InformationDuplicate.txt", "Persnol_Information.txt");

    pthread_mutex_unlock(&mx1);
    
    
    return retval;
}





int check_balance(int account_num)
{
    int retval = -1;
    char* buffer = (char *)malloc(240);
   
    int fd = open("Persnol_Information.txt", O_RDONLY, 0);

    if (fd < 0)
    {
        perror("\n>>>---------------------------------------<<<\nCould not open the file containing accounts information.\n");
        exit(1);
    }


    int i = 0;
    while (read(fd, &buffer[i], 1) == 1)
    {   

        if (buffer[i] == '\n' || buffer[i] == 0x0)
        {
            buffer[i] = 0;
         
            char buffer_copy[140];
          
            char *token = strtok(buffer, " "); //account ID
            strcpy(buffer_copy, token);
            strcat(buffer_copy, " ");
            
            int read_accid = atoi(token);
            token = strtok(NULL, " "); //First Name
            strcat(buffer_copy, token);
            strcat(buffer_copy, " ");
            token = strtok(NULL, " "); //Last Name
            strcat(buffer_copy, token);
            strcat(buffer_copy, " ");
            token = strtok(NULL, " "); //Account Type
            strcat(buffer_copy, token);
            strcat(buffer_copy, " ");
            token = strtok(NULL, " "); //Amount

            if (read_accid == account_num)
            {
                int read_amt = atoi(token);
                retval = read_amt;
            }
            
            i = 0;
            continue; 
        }
        i += 1;
    }
    return retval;
}




char * search_account_handler(char *query)
{
    char *send_response  = (char *) malloc(240);
    char *account_buffer = (char *) malloc(240);
    char *account_buffer_copy = (char *) malloc(240);
    char *not_found_response = (char *)malloc(240);
    int found = 0;

    strcpy(send_response, "\n>>>---------------------------------------<<<\n|| Account Number || First Name || Last Name || Account Type || Balance ||\n>>>---------------------------------------<<<\n\n");
    strcpy(not_found_response, "\n>>>---------------------------------------<<<\nThe entered account number does not exist in the records.\n>>>---------------------------------------<<<\n\n");
    
    pthread_mutex_lock(&mx1);

    int fd1 = open("Persnol_Information.txt", O_RDONLY, 0);
    if( fd1 < 0)
    {
        perror("\n>>>---------------------------------------<<<\nCould not open file containing accounts information.\n>>>---------------------------------------<<<\n\n");
    }

    int i = 0;
    while(read(fd1, &account_buffer[i], 1) == 1)
    {
        if (account_buffer[i] == '\n' || account_buffer[i] == 0x0)
        {
            account_buffer[i] = 0;
            strcpy(account_buffer_copy, account_buffer);
            
            char * token = strtok(account_buffer, " ");
            if(strcmp(token, query) == 0)
            {
                found += 1;
                strcat(send_response, account_buffer_copy);
                strcat(send_response, "\n");
            }
            
            i = 0;
            continue;

        }

        i += 1;
    }

    pthread_mutex_unlock(&mx1);
    if(found == 0)
    {
        return not_found_response;
    }
    else if(found == 1)
    {
        return send_response;
    }
    else
    {
        strcat(send_response, "\n\n>>>---------------------------------------<<<\nThe above are the details of the joint account holders\n>>>---------------------------------------<<<\n\n\n");
        return send_response;
    }
}




void delete_account_handler(char *query)
{
    char *buffer_Secure = (char *)malloc(240);
    char *buffer_Persnol = (char *)malloc(240);

    char *buffer_c_Secure = (char *)malloc(240);
    char *buffer_c_Persnol = (char *)malloc(240);

    char *token_num = strtok(query, " ");

    token_num = strtok(NULL, " "); //Account Number
    int account_id = atoi(token_num);
    
    char *token_name = strtok(NULL, " ");//Name 
    
    char *token_uname = strtok(NULL, " ");//User name

    pthread_mutex_lock(&mx3);

    int fd1 = open("Persnol_Information.txt", O_RDONLY, 0777);
    // printf("check2")
    if (fd1 < 0)
    {
        perror("\n>>>---------------------------------------<<<\nCould not open the file containing accounts information.\n>>>---------------------------------------<<<\n\n");
        exit(1);
    }

    int fd2 = open("Persnol_InformationDuplicate.txt", O_WRONLY | O_CREAT, 0777);
 
    if (fd2 < 0)
    {
        perror("\n>>>---------------------------------------<<<\nCould not open the file containing duplicate accounts information.\n>>>---------------------------------------<<<\n\n");
        exit(1);
    }

    int i = 0;
    while (read(fd1, &buffer_Persnol[i], 1) == 1)
    {
        
        if (buffer_Persnol[i] == '\n' || buffer_Persnol[i] == 0x0)
        {
            buffer_Persnol[i] = 0;
            strcpy(buffer_c_Persnol, buffer_Persnol);
            strcat(buffer_c_Persnol, "\n");
           
            char *token_num1 = strtok(buffer_Persnol, " "); //Account ID
            int read_accid = atoi(token_num1);
            
            char *token_name1=strtok(NULL," "); //first name
            
            
        
            if (strcmp(token_name1,token_name)!=0){
                write(fd2, buffer_c_Persnol, strlen(buffer_c_Persnol));
            }
            
            

            i = 0;
            continue;
        }

        i += 1;
    } 
    close(fd1);
    close(fd2);

    int rem = remove("Persnol_Information.txt");
    
    int ren = rename("Persnol_InformationDuplicate.txt", "Persnol_Information.txt");
    
    pthread_mutex_unlock(&mx3);
   
    pthread_mutex_lock(&mx2);

    int fd3 = open("Secure_Information.txt", O_RDONLY, 0);
    
    if (fd3 < 0)
    {
        perror("Could not open the file containing login information.\n");
        exit(1);
    }

    int fd4 = open("Secure_InformationDuplicate.txt", O_WRONLY | O_CREAT, 0777);
    
    if (fd4 < 0)
    {
        perror("Could not open the file containing duplicate login information.\n");
        exit(1);
    }
    // printf("check1")
    i = 0;
    while (read(fd3, &buffer_Secure[i], 1) == 1)
    {
        if (buffer_Secure[i] == '\n' || buffer_Secure[i] == 0x0)
        {
            buffer_Secure[i] = 0;
            strcpy(buffer_c_Secure, buffer_Secure);
            strcat(buffer_c_Secure, "\n");

            char *token_uname1 = strtok(buffer_Secure, " "); //Username
            char *token_login = strtok(NULL, " "); //Password
            token_login = strtok(NULL, " "); //User Type
            token_login = strtok(NULL, " "); //Account Number
            int accid = atoi(token_login);

            if(strcmp(token_uname1,token_uname)!=0)
            {   
                    write(fd4, buffer_c_Secure , strlen(buffer_c_Secure));
            }

            i = 0;
            continue;
        }

        i += 1;
    }
    // printf("check3")
    close(fd3);
    close(fd4);

    remove("Secure_Information.txt");
    rename("Secure_InformationDuplicate.txt", "Secure_Information.txt");
  
    pthread_mutex_unlock(&mx2);
}




void modify_account_handler(char *query)
{
    char *query1 = strtok(query, " "); //Modify
    query1 = strtok(NULL, " "); //Username
    char *username1 = (char *)malloc(240);
    strcpy(username1, query1);
    query1 = strtok(NULL, " "); // Previous Password
    query1 = strtok(NULL, " "); // New Password

    pthread_mutex_lock(&mx2);

    int fd3 = open("Secure_Information.txt", O_RDONLY, 0777);
    
    if (fd3 < 0)
    {
        perror("Could not open the file containing login information.\n");
        exit(1);
    }

    int fd4 = open("Secure_InformationDuplicate.txt", O_WRONLY | O_CREAT, 0777);
    
    if (fd4 < 0)
    {
        perror("\n>>>-----------------------<<<\nCould not open the file containing duplicate login information.\n>>>-----------------------<<<\n");
        exit(1);
    }
    
    char *buffer_login = (char *)malloc(240);
    char *buffer_response_login = (char *)malloc(240);
    char *buffer_copy_login = (char *)malloc(240);
    
    
    int i = 0;
    while (read(fd3, &buffer_login[i], 1) == 1)
    {
        if (buffer_login[i] == '\n' || buffer_login[i] == 0x0)
        {
            buffer_login[i] = 0;
            strcpy(buffer_copy_login, buffer_login);
            strcat(buffer_copy_login, "\n");

            char *token_login = strtok(buffer_login, " "); //Username
            strcpy(buffer_response_login, token_login);
            strcat(buffer_response_login, " ");

            if(strcmp(token_login, username1) == 0)
            {   
                // printf("first 1\n");
                
                // printf("Should be new pass : %s\n", token_query);

                strcat(buffer_response_login, query1); //New Password
                strcat(buffer_response_login, " ");
                // printf("Resp after new pass : %s\n", buffer_response_login);

                token_login = strtok(NULL, " "); //Password

                token_login = strtok(NULL, " "); //User Type
                strcat(buffer_response_login, token_login);
                strcat(buffer_response_login, " ");

                token_login = strtok(NULL, " "); //Account Number
                strcat(buffer_response_login, token_login);
                strcat(buffer_response_login, "\n");

                write(fd4, buffer_response_login, strlen(buffer_response_login));
            }
            else
            {
               
                write(fd4, buffer_copy_login, strlen(buffer_copy_login));
            }
            
            i = 0;
            continue;
        }

        i += 1;
    }

    close(fd3);
    close(fd4);

    remove("Secure_Information.txt");
   
    rename("Secure_InformationDuplicate.txt", "Secure_Information.txt");
   
    pthread_mutex_unlock(&mx2);
}





void add_account_handler(char *query)
{   
    int acc_number;
    int amount;
    
    char * buffer_copy_login = (char *) malloc(240);
    char * buffer_copy_account = (char *) malloc(240);

    char *token = strtok(query," ");

    token = strtok(NULL, " "); //Username
    strcat(buffer_copy_login, token); 
    strcat(buffer_copy_login, " ");

    token = strtok(NULL, " ");
    strcat(buffer_copy_login, token); //Password
    strcat(buffer_copy_login, " ");

    token = strtok(NULL, " ");
    strcat(buffer_copy_login, token); //User Type
    strcat(buffer_copy_login, " ");

    token = strtok(NULL, " ");
    acc_number = atoi(token);
    strcat(buffer_copy_login, token); //Account Number
    strcat(buffer_copy_login, "\n");

    strcat(buffer_copy_account, token);
    strcat(buffer_copy_account, " ");

    pthread_mutex_lock(&mx2);

    int fd1 = open("Secure_Information.txt",O_CREAT|O_WRONLY, 0777);
    // printf("fd1 : %d\n", fd1);
    if (fd1 < 0)
    {
        perror("Could not open the file containing login information.\n");
        // return NULL;
    }

    lseek(fd1, 0L, SEEK_END);
    write(fd1, buffer_copy_login, strlen(buffer_copy_login));

    pthread_mutex_unlock(&mx2);

    token = strtok(NULL, " "); //First Name
    strcat(buffer_copy_account, token);
    strcat(buffer_copy_account, " ");

    token = strtok(NULL, " "); //Last Name
    strcat(buffer_copy_account, token);
    strcat(buffer_copy_account, " ");

    token = strtok(NULL, " "); //Account Type
    strcat(buffer_copy_account, token);
    strcat(buffer_copy_account, " ");

    token = strtok(NULL, " "); //Initial Deposit
    amount = atoi(token);
    strcat(buffer_copy_account, token);
    strcat(buffer_copy_account, "\n");

    pthread_mutex_lock(&mx3);

    int fd2 = open("Persnol_Information.txt",O_CREAT| O_WRONLY, 0777);
    
    if (fd2 < 0)
    {
        perror("Could not open the file containing accounts information.\n");
        // return NULL;
    }

    lseek(fd2, 0L, SEEK_END);
    write(fd2, buffer_copy_account, strlen(buffer_copy_account));

    pthread_mutex_unlock(&mx3);

}





void admin_handler(char *username, char *password, int unique_account_num, int clientfd)
{
    //printf("second pass");
    int track_session = 1;
    int retval;
    send_to_client(clientfd, "\n>>>-----------------------<<<\n---------------HELLO ADMIN-----------\n>>>-----------------------<<<\nDo you want to:\n1.Add Queries\n2. Delete Queries\n3. Modify Queries\n4. Search for a account details\n5. Exit\nPlease enter number of your choice: \n>>>-----------------------<<<\n\n");

    char *response = (char *)malloc(240);
    
    char *acc_id = (char *)malloc(240);
    char *search_response = (char *)malloc(240);
    char *combined_response = (char *)malloc(240);
    
    while (track_session == 1)
    {
        int choice;
        response = receive_from_client(clientfd);
        if (response == NULL)
        {
            printf(">>>-----------------------<<<\nClient has disconnected abruptly.\n>>>-----------------------<<<\n\n");
            pthread_exit(NULL);
        }

        choice = atoi(response);
        int retval=0;
        //printf("first pass");
        switch (choice)
        {
        case 1:
            pthread_mutex_lock(&mx1);

            int fd1 = open("Admin_work.txt", O_RDONLY, 0);
            
            if (fd1 < 0)
            {
                perror("\n>>>-----------------------<<<\nCould not open the file containing query requests to Admin.\n>>>-----------------------<<<\n");
                exit(1);
            }

            int fd2 = open("Admin_work_Duplicate.txt", O_WRONLY | O_CREAT, 0777);
            
            if (fd2 < 0)
            {
                perror("\n>>>-----------------------<<<\nCould not create duplicate file containing query requests to Admin.\n>>>-----------------------<<<\n");
                exit(1);
            }
            
            char *buffer0 = (char *)malloc(240);
            int i = 0;
            int found1=0;
            
            while (read(fd1, &buffer0[i], 1) == 1)
            {
                if (buffer0[i] == '\n' || buffer0[i] == 0x0)
                {
                    buffer0[i] = 0;
                    
                    if (buffer0[0] == 'A')
                    {
                        found1 += 1;
                        add_account_handler(buffer0);
                    }
                    else
                    {
                        strcat(buffer0,"\n");
                        write(fd2, buffer0, strlen(buffer0));
                    }
                    
                    i = 0;
                    continue;
                }
                i+=1;
            }

            if (found1 == 0)
            {
                retval = 0;
            }
            else
            {
                retval = 1;
            }
            //printf("last pass");
            close(fd1);
            close(fd2);

            remove("Admin_work.txt");
            
            rename("Admin_work_Duplicate.txt", "Admin_work.txt");
           
            pthread_mutex_unlock(&mx1);
            break;
            
            
       case 2:
            pthread_mutex_lock(&mx1);

            int fd10 = open("Admin_work.txt", O_RDONLY, 0);
            
            if (fd10 < 0)
            {
                perror("\n>>>-----------------------<<<\nCould not open the file containing query requests to Admin.\n>>>-----------------------<<<\n");
                exit(1);
            }

            int fd20 = open("Admin_work_Duplicate.txt", O_WRONLY | O_CREAT, 0777);
            
            if (fd20 < 0)
            {
                perror("\n>>>-----------------------<<<\nCould not create duplicate file containing query requests to Admin.\n>>>-----------------------<<<\n");
                exit(1);
            }

            char *buffer100 = (char *)malloc(240);
            int j = 0;
            int found2=0;
            
            while (read(fd10, &buffer100[j], 1) == 1)
            {
                if (buffer100[j] == '\n' || buffer100[j] == 0x0)
                {
                    buffer100[j] = 0;

                    if (buffer100[0] == 'D')
                    {
                        found2 += 1;
                        delete_account_handler(buffer100);
                    }
                    else
                    {
                        strcat(buffer100, "\n");
                        write(fd20, buffer100, strlen(buffer100));
                    }

                    j = 0;
                    continue;
                }
                j+=1;
            }

            if (found2 == 0)
            {
                retval = 0;
            }
            else
            {
                retval = 1;
            }
            
            close(fd10);
            close(fd20);

            remove("Admin_work.txt");
            
            rename("Admin_work_Duplicate.txt", "Admin_work.txt");
            
            // printf("lastpass\n");
            pthread_mutex_unlock(&mx1);
            break;
            
            
            
            case 3:
            pthread_mutex_lock(&mx1);
            //printf("1 first pass");
            int fd31 = open("Admin_work.txt", O_RDONLY, 0777);
            
            if (fd31 < 0)
            {
                perror("Could not open the file containing query requests to Admin.\n");
                exit(1);
            }

            int fd32 = open("Admin_work_Duplicate.txt", O_WRONLY | O_CREAT, 0777);
            
            if (fd32< 0)
            {
                perror("Could not create duplicate file containing query requests to Admin.\n");
                exit(1);
            }

            char *buffer101 = (char *)malloc(240);
            int t = 0;
            int found3=0;
            
            while (read(fd31, &buffer101[t], 1) == 1)
            {
            printf("2 first pass");
                if (buffer101[t] == '\n' || buffer101[t] == 0x0)
                {
                    buffer101[t] = 0;

                    if (buffer101[0] == 'M')
                    {
                        found3 += 1;
                        modify_account_handler(buffer101);
                    }
                    else
                    {
                        strcat(buffer101, "\n");
                        write(fd32, buffer101, strlen(buffer101));
                    }

                    t = 0;
                    continue;
                }
                t+=1;
            }
            
            if (found3 == 0)
            {
                retval = 0;
            }
            else
            {
                retval = 1;
            }
            close(fd31);
            close(fd32);
            //printf("third pass %d\n",retval);
            int rem31 = remove("Admin_work.txt");
            int ren31 = rename("Admin_work_Duplicate.txt", "Admin_work.txt");
            pthread_mutex_unlock(&mx1);
            break;


            case 4:
                send_to_client(clientfd, "\n>>>-----------------------<<<\nEnter account number which you want to know its information: \n>>>-----------------------<<<\n");
                acc_id = receive_from_client(clientfd);
                if (acc_id == NULL)
                {
                    printf("\n>>>-----------------------<<<\nClient has disconnected abruptly.\n>>>-----------------------<<<\n\n");
                    pthread_exit(NULL);
                }

                search_response = search_account_handler(acc_id);
                strcat(search_response, "\n>>>-----------------------<<<\n>>>-----------------------<<<\nIf you wish to perform some other operation , please enter the option number again : \n>>>-----------------------<<<\n>>>-----------------------<<<\n\n");
                    send_to_client(clientfd, search_response);
                break;
                
           
            case 5:
                track_session = 0;
                break;
        }
        if(retval == 1)
        {
                send_to_client(clientfd, "Queries successfully executed\n---------------------------------------------------------\nIf you wish to perform some other operation , please enter the option number again : \n\n");
        }
        else
        {
                if(choice == 1)
                {
                     send_to_client(clientfd, "No pending Add queries\n----------------------------------------------------------------\nIf you wish to perform some other operation, please enter the option number again : \n\n");
                }
                    
                else if (choice == 2)
                {
                      send_to_client(clientfd, "\n>>>-----------------------<<<\n>>>-----------------------<<<\nNo pending Delete queries\n-----------------------------------------\nIf you wish to perform some other operation , please enter the option number again : \n>>>-----------------------<<<\n>>>-----------------------<<<\n\n");
                }

                else if (choice == 3)
                {
                      send_to_client(clientfd, "\n>>>-----------------------<<<\n>>>-----------------------<<<\nNo pending Modify queries\n---------------------------------------------\nIf you wish to perform some other operation , please enter the option number again : \n>>>-----------------------<<<\n>>>-----------------------<<<\n\n");
                }
                else if (choice==4)
                {
                      send_to_client(clientfd, "\n>>>-----------------------<<<\n>>>-----------------------<<<\nIf you wish to perform some other operation , please enter the option number again : \n>>>-----------------------<<<\n>>>-----------------------<<<\n\n");
                }
                else
                {
                    send_to_client(clientfd, "\n>>>-----------------------<<<\n>>>-----------------------<<<\nPlease Press 'q' for exit...Admin are signoff.... : \n>>>-----------------------<<<\n>>>-----------------------<<<\n\n");
                }
                
               
         }
    }
}







void user_handler(char *username, char* password, int unique_account_id, int clientfd,int ty)
{   
    int session = 1;
    int type=ty;
    send_to_client(clientfd, "\n>>>>----------------------------------------------------<<<<\n>>>>-----------WELCOME USER------------------<<<<<\nDo you want to:\n............\n1. Deposit\n2. Withdraw\n3. Balance Enquiry\n4. Password Change\n5. View Details\n6. Delete Account\n7. Exit\nPlease enter number of your choice: \n>>>>--------------------------<<<<\n\n");

    char *response = (char *)malloc(240);
    while(session == 1)
    {
        int choice;
        response = receive_from_client(clientfd);
        if (response == NULL)
        {
            printf("\n>>------------------<<\nClient has disconnected abruptly.\n>>------------------<<\n");
            pthread_exit(NULL);
        }

        choice = atoi(response);

        
        int operation;
        int amount;
        char *x = (char *)malloc(240);
        char *amt = (char *)malloc(240);

        char *old_password = (char *)malloc(240);
        char *new_password = (char *)malloc(240);
        char *confirm_password = (char *)malloc(240);

        int deposit_withdraw_ret;
        int balance;
        char *balance_str = (char *)malloc(240);

        
        char *query_buff = (char *)malloc(240);

        char *search_response = (char *)malloc(240);
        
        int account_num;
        switch (choice)
        {
            //DEposite
            case 1:
                operation = 1;
                
                send_to_client(clientfd, "\n--------------------------\nEnter your account number: \n--------------------------\n");
                x = receive_from_client(clientfd);
                if (x == NULL)
                {
                    printf("\n;(--------------------------);\n\n;(--------------------------);\nClient has disconnected abruptly.\n;(--------------------------);\n\n");
                    pthread_exit(NULL);
                }

                account_num = atoi(x);
                if(account_num != unique_account_id)
                {
                    send_to_client(clientfd, "\n--------------------------\nIncorrect Account ID!\nPlease choose the operation to perform again :\n--------------------------\n\n");
                    // pthread_exit(NULL);
                    break;  
                }

                send_to_client(clientfd, "\n\n----------------------\nEnter amount to deposit:\n---------------------- \n");
                amt = receive_from_client(clientfd);
                if (amt == NULL)
                {
                    printf("\n;(--------------------------);\n;(--------------------------);\nClient has disconnected abruptly.\n;(--------------------------);\n\n");
                    pthread_exit(NULL);
                }

                amount = atoi(amt);
                
                //Deposite Function
                deposit_withdraw_ret = deposit_withdraw_handler(account_num, operation, amount,type);
                if(deposit_withdraw_ret == 0)
                {
                    send_to_client(clientfd, "\n---------------------------------------------------------------------------\n---------------------------------------------------------------------------\n>>>----------------------Notification From Bank-----------------------<<<\n---------------------------------------------------------------------------\nMoney Deposited Successfully\n\nIf you wish to perform any other operation, please enter the number of your choice again : \n\n----------->>Other wise Press 7 For Exit<<-----------\n----------->>BE SAFE THANKYOU FOR VISIT<<-----------\n");
                }
                else
                {
                    close_connection(clientfd, ";(--------------------------);Transaction Failed\nPlease try Again Later\n\nPress 'q' to quit.\n>>------------------------<<\n");
                    pthread_exit(NULL);
                }
                
                break;
            //Withdrwal
            case 2:
                operation = 2;

                send_to_client(clientfd, "\n>>>---------------------<<<\nEnter your account number:\n>>>---------------------<<< \n");
                x = receive_from_client(clientfd);
                if (x == NULL)
                {
                    printf("\n;(--------------------------);\n;(--------------------------);\nClient has disconnected abruptly.\n;(--------------------------);\n\n");
                    pthread_exit(NULL);
                }

                account_num = atoi(x);
                if (account_num != unique_account_id)
                {
                    send_to_client(clientfd, "\n>>>---------------------<<<\nIncorrect Account ID!\n>>>---------------------<<<\nPlease choose the operation to perform again :\n>>>---------------------<<< \n\n");
                    
                    break;
                }

                send_to_client(clientfd, "\n>>>---------------------<<<\nEnter amount to withdraw: \n>>>---------------------<<<\n\n");
                amt = receive_from_client(clientfd);
                if (amt == NULL)
                {
                    printf("/n---------------------\nClient has disconnected abruptly.\n-----------------------\n\n");
                    pthread_exit(NULL);
                }

                amount = atoi(amt);
                //withdrawl function
                deposit_withdraw_ret = deposit_withdraw_handler(account_num, operation, amount,type);
                if (deposit_withdraw_ret == -1)
                {
                    send_to_client(clientfd, "\n-------------------------------------------------------------------------\n--------------------------------------------------------------------------\n>>>----------------------Notification From Bank--------------------------<<<\nMoney Withdrawl Decline\n>>>---------------------<<<\nIf you wish to perform any other operation, please enter the number of your choice again : \n>>>----------------------Other wise Press 7 For Exit<<<----------------------\n>>>----------------------BE SAFE THANKYOU FOR VISIT<<<---------------------->>>\n");
                }
                else
                {
                    send_to_client(clientfd, "\n--------------------------------------------------------------------------\n--------------------------------------------------------------------------\n>>>----------------------Notification From Bank--------------------------<<<\nMoney Withrwal Successfully>>>---------------------<<<\nIf you wish to perform any other operation, please enter the number of your choice again : \n>>>----------------------Other wise Press 7 For Exit<<<----------------------\n>>>----------------------<<<BE SAFE THANKYOU FOR VISIT<<<---------------------->>>\n");
                }
                
                break;

            case 3:
                operation = 3;
                

                send_to_client(clientfd, "\n>>--------------------<<\nEnter your account number: \n>>--------------------<<\n\n");
                x = receive_from_client(clientfd);
                if (x == NULL)
                {
                    printf("Client has disconnected abruptly.\n\n");
                    pthread_exit(NULL);
                }

                account_num = atoi(x);
                if (account_num != unique_account_id)
                {
                    send_to_client(clientfd, "\n------------------------\nIncorrect Account ID!\nPlease choose the operation to perform again : \n----------------------------\n\n");
                    break;
                }

                balance = check_balance(account_num);

                if(balance == -1)
                {
                    close_connection(clientfd,"\n>>--------------------<<\nThe Account Balance are Zero!\nPlease Maintance Minimun balance, Otherwise account are Deleted!! \nPress 'q' to quit.\n\n>>--------------------<<\n\n");
                    pthread_exit(NULL);
                }
                else
                {
                    sprintf(balance_str, "%d", balance);
                    strcat(balance_str, " is your balance\nIf you wish to perform any other operation, please enter the number of your choice again : \n>>>>------------------------------<<\n");
                    send_to_client(clientfd, balance_str);
                }
                
                break;

            case 4:

                send_to_client(clientfd, "\n>>>---------------<<<\nEnter old password:\n >>>---------------<<<\n");
                old_password = receive_from_client(clientfd);
                if (old_password == NULL)
                {
                    printf("\n >>>---------------<<<\nClient has disconnected abruptly.\n >>>---------------<<<\n");
                    pthread_exit(NULL);
                }

                if(strcmp(old_password, password) == 0)
                {
                
                
                    send_to_client(clientfd, "\n>>>---------------<<<\nEnter new password:\n >>>---------------<<<\n");
                    new_password = receive_from_client(clientfd);
                    if (new_password == NULL)
                    {
                        printf("\n >>>---------------<<<\nClient has disconnected abruptly.\n >>>---------------<<<\n");
                        pthread_exit(NULL);
                    }

                    send_to_client(clientfd, "\n>>>---------------<<<\nConfirm new password: \n >>>---------------<<<\n");
                    confirm_password = receive_from_client(clientfd);
                    if (confirm_password == NULL)
                    {
                        printf("\n >>>---------------<<<\nClient has disconnected abruptly.\n >>>---------------<<<\n\n");
                        pthread_exit(NULL);
                    }

                    if(strcmp(new_password, confirm_password) == 0)
                    {
                        pthread_mutex_lock(&mx1);
                        int fd1 = open("Admin_work.txt", O_WRONLY | O_CREAT, 0777);
                        if(fd1 < 0)
                        {
                            printf("\n >>>---------------<<<\nCould not open the file containing requests to Admin.\n >>>---------------<<<\n");
                        }
                        strcpy(query_buff, "Modify ");
                        strcat(query_buff,username);
                        strcat(query_buff, " ");
                        strcat(query_buff, password);
                        strcat(query_buff, " ");
                        strcat(query_buff, new_password);
                        strcat(query_buff, "\n");
                        
                        lseek(fd1, 0L, SEEK_END);
                        write(fd1, query_buff, strlen(query_buff));
                        pthread_mutex_unlock(&mx1);
                        close_connection(clientfd, "\n >>>-------------------------------<<<\n>>>-------------------------------<<<\nYour query has been successfully added to the query list for the Admin to be reviewed.\nWe expect your query to be reviewed within 1 Working  day.\nPress 'q' to quit.\n>>>-------------------------------<<<\n>>>-------------------------------<<<\n");
                        pthread_exit(NULL);
                        
                        break;
                    }
                    else
                    {
                        send_to_client(clientfd, "/n>>>-------------------------------<<<\nIncorrect confirmation password.\n------------------------------------\nPlease choose same option and try again and make sure your confirmation password are same\n>>>-------------------------------<<<\n");
                        break;
                    }
                    
                }
                else
                {
                    send_to_client(clientfd, "\n>>>-------------------------------<<<\nThe password you have entered is incorrect.\n>>>-------------------------------<<<\nYour password are inccorect Please choose the 'Password Change' option and try again!\n>>>-------------------------------<<<\n");
                }
                
                break;

            case 5:
                operation = 5;

                send_to_client(clientfd, "\n>>-----------------------<<<\nEnter your account number:\n>>>------------------------<< \n");
                x = receive_from_client(clientfd);
                if (x == NULL)
                {
                    printf("\n>>-----------------------<<<\nClient has disconnected abruptly.\n>>>-----------------------<<<\n\n");
                    pthread_exit(NULL);
                }

                account_num = atoi(x);
                if (account_num != unique_account_id)
                {
                    send_to_client(clientfd, "\n>>-----------------------<<<\nIncorrect Account ID!\n----------------------\nPlease rechoose the option and try again\n>>>-----------------------<<<\n");
                    break;
                }
                else
                {
                    search_response = search_account_handler(x);
                    strcat(search_response, "\n>>-----------------------<<<\n>>>-----------------------<<<\nIf you wish to perform any other operation, please enter the number of your choice again : \n>>>-----------------------<<<\n>>-----------------------<<<\n");
                    send_to_client(clientfd, search_response);
                }

                break;

            case 6:
                operation = 6;

                send_to_client(clientfd, "\n>>-----------------------<<<\nEnter your account number:\n>>-----------------------<<< \n");
                char *x = (char *)malloc(240);
                x = receive_from_client(clientfd);
                
                char *name=(char *)malloc(240);
                char *u_name=(char *)malloc(240);
                send_to_client(clientfd, "\n>>-----------------------<<<\nEnter your account name:\n>>-----------------------<<< \n");
                name=receive_from_client(clientfd);
                send_to_client(clientfd, "\n>>-----------------------<<<\nEnter your account user name:\n>>-----------------------<<< \n");
                u_name=receive_from_client(clientfd);
                if (x == NULL)
                {
                    printf("\n>>>-----------------------<<<\nClient has disconnected abruptly.\n>>>-----------------------<<<\n\n");
                    pthread_exit(NULL);
                }

                account_num = atoi(x);
                
                if (account_num != unique_account_id)
                {
                    send_to_client(clientfd, "\n>>>-----------------------<<<\n>>-----------------------<<<\nIncorrect Account ID!\n----------------------------\nPlease choose the operation to perform again : \n>>>-----------------------<<<\n>>-----------------------<<<\n\n");
                    break;
                }

                pthread_mutex_lock(&mx1);
                int query_fd = open("Admin_work.txt", O_WRONLY | O_CREAT, 0777);
                if (query_fd < 0)
                {
                    printf("\n>>>-----------------------<<<\nCould not open the file containing requests to Admin.\n>>>-----------------------<<<\n");
                }
                strcpy(query_buff, "Delete ");
                strcat(query_buff, x);
                strcat(query_buff, " ");
                strcat(query_buff, name);
                strcat(query_buff, " ");
                strcat(query_buff, u_name);
                strcat(query_buff, "\n");

                lseek(query_fd, 0L, SEEK_END);
                write(query_fd, query_buff, strlen(query_buff));
                pthread_mutex_unlock(&mx1);
                close_connection(clientfd, "\n>>>---------------------------------------------------------<<<\n>>>---------------------------------------------------------<<<\nYour quary for deleting the account are going for acceptance from admin\n>>>---------------------------------------------------------<<<\nIf you have any issue from your branch and their service adn This is your reason for your move\nPlease Contact with our customer care we are happy to help you\n>>>---------------------------------------------------------<<<\nyou Received a conformation call from our executive\nAfter this your account will be deleted in 3 working day\n>>>---------------------------------------------------------<<<\n>>>---------------------------------------------------------<<<\nFor Quiting please press q\n----------------########---------------########------------\n");
                pthread_exit(NULL);
                
                break;


            case 7:
                session = 0;
                break;

                
        }
    }
    }





int* authenticate(char *username, char *password)
{
    int fd;
    int user_type;
    int unique_account_id;
    char *buffer = (char *)malloc(240);
    static int auth_arr[4];
    // printf("frist pass");
    fd = open("Secure_Information.txt", O_RDONLY, 0);
    if (fd <= 0)
    {
        perror("Could not open the file containing login information.\n");
        return NULL;
    }

    int found_username = 0;
    int correct_password = 0;
    int undefined_usertype = 0;

    int i = 0;
    while(read(fd, &buffer[i], 1) == 1)
    {
        // printf("secomd pass");
        if(buffer[i] == '\n' || buffer[i] == 0x0 )
        {
            buffer[i] = 0;

            char *token = strtok(buffer, " ");
            
            if(strcmp(token, username) == 0)
            {
                found_username = 1;
                token = strtok(NULL, " ");
                if(strcmp(token, password) == 0)
                {
                    correct_password = 1;
                    token = strtok(NULL, " ");
                    if(!strncmp(token, "Normal", strlen("Normal")))
                    {
                        user_type = 0;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }

                    else if (!strncmp(token, "Joint", strlen("Joint")))
                    {
                        user_type = 1;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }

                    else if (!strncmp(token, "Admin", strlen("Admin")))
                    {
                        user_type = 2;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }

                    else
                    {
                        user_type = 3;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }
                }
            
                else
                {
                    correct_password = -1;
                    break;
                }
            }

            else
            {
                found_username = 0;
                
            }
            
            i = 0;
            continue;
        }

        i += 1;

    }

    if (found_username == 0)
    {
        found_username = -1;
    }
    
    auth_arr[0] = found_username;
    auth_arr[1] = correct_password;
    auth_arr[2] = user_type;
    auth_arr[3] = unique_account_id;
    close(fd);
    return auth_arr;
}






void *client_handler(void *clientid)
{
    int clientfd = *((int *)clientid);

    char *username;
    char *password;
    

    username = (char *) malloc(240);
    password = (char *) malloc(240);

    //login_function;
    
    char *en_username;
    char *en_password;

    send_to_client(clientfd, "\nEnter Username : ");
    en_username = receive_from_client(clientfd);
    if(en_username == NULL)
    {
        printf("Client has disconnected abruptly.\n\n");
        pthread_exit(NULL);
    }
   
    send_to_client(clientfd, "\nEnter Password : ");
    en_password = receive_from_client(clientfd);
    if (en_password == NULL)
    {
        printf("\nClient has disconnected abruptly.\n\n");
        pthread_exit(NULL);
    }
 
    // copy it to username and password
    int i = 0;
    while(en_username[i] != '\0' && en_username[i] != '\n')
    {
        username[i] = en_username[i];
        i+=1;
    }
    username[i] = '\0';

    i = 0;
    while(en_password[i] != '\0' && en_password[i] != '\n')
    {
        password[i] = en_password[i];
        i += 1;
    }
    password[i] = '\0';
    
    
   
    //Authentication function
       //Authenticstion
    int fdA;
    int user_type;
    int unique_account_id;
    char *bufferA = (char *)malloc(240);
    int auth_values[4];
    // printf("frist pass");
    fdA = open("Secure_Information.txt", O_RDONLY, 0);
    if (fdA <= 0)
    {
        perror("Could not open the file containing login information.\n");
        return NULL;
    }

    int found_username = 0;
    int correct_password = 0;
    int undefined_usertype = 0;

    int S = 0;
    while(read(fdA, &bufferA[S], 1) == 1)
    {
        // printf("secomd pass");
        if(bufferA[S] == '\n' || bufferA[S] == 0x0 )
        {
            bufferA[S] = 0;

            char *token = strtok(bufferA, " ");
            
            if(strcmp(token, username) == 0)
            {
                found_username = 1;
                token = strtok(NULL, " ");
                if(strcmp(token, password) == 0)
                {
                    correct_password = 1;
                    token = strtok(NULL, " ");
                    if(!strncmp(token, "Normal", strlen("Normal")))
                    {
                        user_type = 0;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }

                    else if (!strncmp(token, "Joint", strlen("Joint")))
                    {
                        user_type = 1;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }

                    else if (!strncmp(token, "Admin", strlen("Admin")))
                    {
                        user_type = 2;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }

                    else
                    {
                        user_type = 3;
                        token = strtok(NULL, " ");
                        unique_account_id = atoi(token);
                        break;
                    }
                }
            
                else
                {
                    correct_password = -1;
                    break;
                }
            }

            else
            {
                found_username = 0;
                
            }
            
            S = 0;
            continue;
        }

        S += 1;

    }

    if (found_username == 0)
    {
        found_username = -1;
    }
    
    auth_values[0] = found_username;
    auth_values[1] = correct_password;
    auth_values[2] = user_type;
    auth_values[3] = unique_account_id;
    close(fdA);
 
   
    

    
     

    if(auth_values[0] == -1)
    { 
    
        send_to_client(clientfd, "No such user found!\n\nDo you wish to add a new account? (Y/N)\n");
        char * response = receive_from_client(clientfd);
        if (response == NULL)
        {
            printf("Client has disconnected abruptly.\n\n");
            pthread_exit(NULL);
        }

        if(strcmp(response, "Y") == 0)
        {
            char *new_username = (char *) malloc(240);
            char *new_password = (char *) malloc(240);
            char *new_user_type = (char *) malloc(240);
            char *new_acc_no = (char *) malloc(240);
            int new_acc_number;
            char *new_fname = (char *) malloc(240);
            char *new_lname = (char *) malloc(240);
            char *new_acc_type = (char *) malloc(240);
            char *new_initial_dep = (char *) malloc(240);

            send_to_client(clientfd, "\n<<<---------------------->>>\nEnter the new  username:");
            new_username = receive_from_client(clientfd);
            if (new_username == NULL)
            {
                printf("Client has disconnected abruptly.\n\n");
                pthread_exit(NULL);
            }

            send_to_client(clientfd, "\n<<<---------------------->>>\nEnter a password: ");
            new_password = receive_from_client(clientfd);
            if (new_password == NULL)
            {
                printf("Client has disconnected abruptly.\n\n");
                pthread_exit(NULL);
            }

            send_to_client(clientfd, "\n<<<---------------------->>>\nEnter User type (Normal/Joint) you have chooes only between this :: ");
            new_user_type = receive_from_client(clientfd);
            char arr[15];
            strcpy(arr,"Admin");
            
            if (strcmp(new_user_type,arr)==0)
            {
                close_connection(clientfd, "\n\n>>>>------------------------<<<<\nYou are not able to do this, for this you need special Permissions!\n>>>>------------------------<<<<\nPress 'q' to quit.\n>>>>------------------------<<<\n");
            }
            if (new_user_type == NULL)
            {
                printf("Client has disconnected abruptly.\n\n");
                pthread_exit(NULL);
            }

            if(strcmp(new_user_type,"Joint") == 0)
            {
                send_to_client(clientfd, "\n<<<---------------------->>>\nDetails regarding the exisiting account...\n\nEnter the Joint Account Number::");
                new_acc_no = receive_from_client(clientfd);
                int account_num1=atoi(new_acc_no);
                if (new_acc_no == NULL)
                {
                    printf("Client has disconnected abruptly.\n\n");
                    pthread_exit(NULL);
                }

                send_to_client(clientfd, "\n<<<---------------------->>>\nEnter Joint Account Type (Savings/Current):");
                new_acc_type = receive_from_client(clientfd);
                if (new_acc_type == NULL)
                {
                    printf("Client has disconnected abruptly.\n\n");
                    pthread_exit(NULL);
                }

                send_to_client(clientfd, "\n<<<---------------------->>>\nEnter Balance in the Joint Account: ");
                new_initial_dep = receive_from_client(clientfd);
                int add_amount=atoi(new_initial_dep);
                
       
                //**self
                
                pthread_mutex_lock(&mx1);

                //printf("00000000000000000000000");
                char* buffer11 = (char *)malloc(240);
                
                int fd11 = open("Persnol_Information.txt", O_RDONLY, 0777);
                
                if (fd11 < 0)
                {
                perror("Could not open the file containing accounts information.\n");
                // return NULL;
                }

                int fd22 = open("Persnol_InformationDuplicate.txt", O_WRONLY | O_CREAT, 0777);
                // printf("frist pass");
                
                if (fd22 < 0)
                {
                perror("Could not create duplicate file containing accounts information.\n");
                exit(1);
                }

                int l = 0;
                while (read(fd11, &buffer11[l], 1) == 1)
                {
                if (buffer11[l] == '\n' || buffer11[l] == 0x0)
                {
                     buffer11[l] = 0;
                     //printf("secnf press");
                     char buffer_copy[240];
                     char *token11 = strtok(buffer11, " "); //Account ID
                     strcpy(buffer_copy, token11);
                     strcat(buffer_copy, " ");
                     int read_accid = atoi(token11);
                     token11 = strtok(NULL, " "); //First Name
                     strcat(buffer_copy, token11); 
                     strcat(buffer_copy, " ");
                     token11 = strtok(NULL, " "); //Last Name
                     strcat(buffer_copy, token11); 
                     strcat(buffer_copy, " ");
                     token11 = strtok(NULL, " "); //Account Type
                     strcat(buffer_copy, token11); 
                     strcat(buffer_copy, " ");  
                     token11 = strtok(NULL, " "); //Amount
                     
                     //asign token to amount which are going to add in account information

                if(read_accid == account_num1)
                {
                     //printf("dooooooooooonnnnnnnnn8433");
                     int read_amt = atoi(token11);
                     int new_amt;

                     new_amt = read_amt + add_amount;
                     char* new_amt_str = (char *)malloc(240);
                
                     sprintf(new_amt_str, "%d", new_amt);
                     new_initial_dep=new_amt_str;
                
                     strncpy(token11, new_amt_str, strlen(new_amt_str)); 
                     int n = strlen(token11) - strlen(new_amt_str);
                    
                    if(n > 0)
                    {
                         for (int y = 0; y < n; y++)
                         {
                               token11[strlen(new_amt_str) + y] = '\0';
                         }
                    }
                }
                strcat(buffer_copy, token11); 
                strcat(buffer_copy, "\n"); 
                // printf("third pass pass");
                int w = write(fd22, buffer_copy, strlen(buffer_copy));
                // printf("Write : %d\n", w);

                l = 0;
                continue;
               }
               l += 1;
               }
              // printf("lst pass");
               close(fd11);
               close(fd22);
               int rem12 = remove("Persnol_Information.txt");
               
               int ren12 = rename("Persnol_InformationDuplicate.txt", "Persnol_Information.txt");
               
               pthread_mutex_unlock(&mx1);
                
               //self*
         
                if (new_initial_dep == NULL)
                {
                    printf("Client has disconnected abruptly.\n\n");
                    pthread_exit(NULL);
                }
            }

            else
            {
                srand(time(NULL));
                new_acc_number = rand() % 10000 + 1;
                sprintf(new_acc_no, "%d", new_acc_number);
           
                send_to_client(clientfd, "\n<<<<--------------------->>>\nEnter Account type (Savings/Current): ");
                new_acc_type = receive_from_client(clientfd);
                if (new_acc_type == NULL)
                {
                    printf("Client has disconnected abruptly.\n\n");
                    pthread_exit(NULL);
                }

                send_to_client(clientfd, "\n<<<<--------------------->>>\nEnter Initial Deposit money how much you want to deposite: ");
                new_initial_dep = receive_from_client(clientfd);
                if (new_initial_dep == NULL)
                {
                    printf("Client has disconnected abruptly.\n\n");
                    pthread_exit(NULL);
                }
            }
            
            
            send_to_client(clientfd, "\n<<<<--------------------->>>\nEnter your First Name: ");
            new_fname = receive_from_client(clientfd);
            if (new_fname == NULL)
            {
                printf("Client has disconnected abruptly.\n\n");
                pthread_exit(NULL);
            }

            send_to_client(clientfd, "\n<<<<--------------------->>>\nEnter your Last Name: ");
            new_lname = receive_from_client(clientfd);
            if (new_lname == NULL)
            {
                printf("Client has disconnected abruptly.\n\n");
                pthread_exit(NULL);
            }

            
            pthread_mutex_lock(&mx1);
            
            int query_fd = open("Admin_work.txt", O_WRONLY | O_CREAT, 0777);
            if (query_fd < 0)
            {
                printf("Could not open the file containing requests to Admin.\n");
            }
            
            char *query_buff = (char *)malloc(140);
            strcpy(query_buff, "Add ");
            strcat(query_buff, new_username);
            strcat(query_buff, " ");
            strcat(query_buff, new_password);
            strcat(query_buff, " ");
            strcat(query_buff, new_user_type);
            strcat(query_buff, " ");
            strcat(query_buff, new_acc_no);
            strcat(query_buff, " ");
            strcat(query_buff, new_fname);
            strcat(query_buff, " ");
            strcat(query_buff, new_lname);
            strcat(query_buff, " ");
            strcat(query_buff, new_acc_type);
            strcat(query_buff, " ");
            strcat(query_buff, new_initial_dep);
            strcat(query_buff, "\n");

            // printf("%s\n", query_buff);

            lseek(query_fd, 0L, SEEK_END);
            write(query_fd, query_buff, strlen(query_buff));
            pthread_mutex_unlock(&mx1);

            strcat(new_acc_no, "\n<<<<------------------------------------------------>>>\n<<<<------------------------------------------------>>>\n<<<<----->>>>>Thank you for creating Account<<<<----->>>>> \n<<<<------------------------------------------------>>>\n<<<<------------------------------------------------>>>\n\n\n<<<<----->>>>>Your Account are activated after Three day<<<<------>>>>\n<<<<------------------------------------------------>>>\n<<<<------------------------------------------------>>>\n<<<<----->>>>Our welcom kit are courier in one week<<<<----->>>>\n<<<<----->>>>>THANKYOU FOR SELECTING US PLEASE PRESS Q<<<<----->>>>\n<<<<------------------------------------------------>>>\n<<<<------------------------------------------------>>>\n");
            close_connection(clientfd, new_acc_no);
        }
        else
        {
            close_connection(clientfd, "\n;)--------------------------(;\nThank you for visiting!\n<------------------------->\nPress 'q' to quit.\n;)--------------------------(;\n");
        }
        pthread_exit(NULL);
    } 

    if(auth_values[1] == -1)
    {
        close_connection(clientfd, "<<--------------------------->>Incorrect password entered!\n<<---------------------------->>\nPress 'q' to quit\n\n");
        pthread_exit(NULL);
    }
    int p;
    switch(auth_values[2])
    {
        case 0:
            // printf("Normal User\n");
            p=0;
            user_handler(username, password, unique_account_id, clientfd,p);
            close_connection(clientfd, "Thank you for visiting us!\n----------------------------------\nPlease do come back again.\n----------------------------------\nPress 'q' to quit.\n\n");
            break;

        case 1:
            // printf("Joint User\n");
            p=1;
            user_handler(username, password, unique_account_id, clientfd,p);
            close_connection(clientfd, "Thank you for visiting us!\nPlease do come back again.\n----------------------------------\n-------------------------------\nPress 'q' to quit.\n\n");
            break;

        case 2:
            // printf("Admin User\n");
            admin_handler(username, password, unique_account_id, clientfd);
            close_connection(clientfd, "Thank you for visiting us!\n----------------------------------\nPlease do come back again.\n----------------------------------\nPress 'q' to quit.\n\n");
            break;

        case 3:
            close_connection(clientfd, "You are unauthorized! This type of account are not Exist\n--------------------------------------------\nPress 'q' to quit.\n\n");
            pthread_exit(NULL);
            // printf("You are unauthorized!\n");
            break;
    }

    pthread_exit(NULL);
}



int main(int argc, char **argv)
{

    while (1) {
    
  
    pthread_mutex_init(&mx1, NULL);
    pthread_mutex_init(&mx2, NULL);
    pthread_mutex_init(&mx3, NULL);

        
    printf("SERVER...\n");
    
    if(argc < 2)
    {
        printf("Insufficient Number of arguments\nFor example ./a.out <port_number>\n");
        exit(0);
    }

    
   
    int serverfd;
    int clientid;
    int port_number;

    struct sockaddr_in server;
    struct sockaddr_in client;
    socklen_t address_size;
    
   
    port_number = atoi(argv[1]);

    serverfd = socket(AF_INET, SOCK_STREAM,0);
    if(serverfd == -1)
    {
        perror("Socket Error...Socket doesnt creat....\n");
        exit(0);
    }

    memset((void *)&server, 0, sizeof(server));
    
    server.sin_family = AF_INET;
    server.sin_port = htons(port_number);
    server.sin_addr.s_addr = INADDR_ANY;                            

   if (signal(SIGINT, handler) == SIG_ERR)
   {
        printf("Error in catching SIGINT\n");
        exit(1);
   }
    
    
    int bnd = bind(serverfd, (struct sockaddr*)(&server), sizeof(server));
    
    listen(serverfd,10);
    printf("Clients queue size upto 10\n");
    
    pthread_t thread[30];
    int visitors=0;
    int i = 0;
    address_size=sizeof(client);
    while(1)
    {
        memset((void *)&client, 0 , sizeof(client));

        int clientid = accept(serverfd, (struct sockaddr*)(&client), &address_size);
        
        visitors += 1;

        printf("Connected to client: %s\n", inet_ntoa(client.sin_addr));
        printf("Total number of visitors : %d\n",visitors);
        
        if (pthread_create( &thread[i++], NULL, client_handler, &clientid) != 0 ){
            perror("Could not create thread.\n ");
            }        
    }
    exit(1);
  }
    return 0;
}

