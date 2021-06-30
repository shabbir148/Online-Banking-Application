                                                            ------------------------------------------------
                                                            MINI-PROJECT :: ONLINE_BANKING_MANAGEMENT_SYSTEM
                                                            ------------------------------------------------




1. Database :

    Database are important part for our project for which i used .txt formate to store all related information which are going to produce or will be use for Banking Managment system. For which was 
    creat 3 file name as:
    
    Persnol_Information.txt
        Its contain authorize detail about customers and also show remaning balance after making transation from account, Means if i will do any withdrawl and deposite opration in my account it
        reflected in this file.
        
        Information is in the form -> 
        
         -> Unique Account Number 
         -> First Name 
         -> Last Name 
         -> Account Type 
         -> Balance 

    Transection_detail.txt
        Its contain authoriz transection from account with specific data and day are also mentioned
        -->>Date when Transection are happend
        -->>Account Number from which Transection are going on
        -->>Account Holder Name
        -->>Amount of Withdrawl/Deposite Money
    
    
    Secure_Information.txt
        Information like Password and Userid are very crucial for any account so i will be decided that make a another file with low level permission like only admin or higher authority make a change
        on it so was create this file with permission 0744, only admin have read, write and execute permission.
        In this i will be going to store all crucial information which are very important for security purpose, in file name Secure_information.
        Contains information regarding the accounts of customers.
        
        Details are in the form -> 
        
        -> Username
        -> Password
        -> User Type
        -> Unique Account Number


    Admin_work.txt
        This file are only made for admin work or admin task. In by this file admin saw what Task/Request are incomplite like changing password Request, Add new account and all this type of staff. 
       
       
2. This Online_Bankiing System are Load with Many Feature, Some Key feature as follows :

    Firstly simple login prompt are open in which, They will have asked about Username and Password, than this Username and Password are going for verification or authentication purpose, after this 
    they identify automecticaly that you are client or Admin.
    
    -->> key feature: 
        # Authoriz transection from account with specific data and day are also mentioned in the file name Transection_detail.txt
        # They identify automecticaly user are admin or customer by combination of Password and username smartly.
        # Ambiguities in in handling Joint account are ver efficaintly handle like if creat account and it will have joint account than assign same amount which are exit in old user to new account  
          which are creat resendly, or when we perform operation like deposit and withdrawl are perfectly reflect back to both accounts detial, we do this type of information are very efficaintly. 
        # Checks error messages to take care of incorrect information such as incorrect Username,password and confirmation passwords.
        # If you make mistake unintentionally than then code provide re-prompting/re-entering wherever its possible
        # SIGINT handler to take care of abrupt disconnections. (via Ctrl-C)
        # For tcp protocole we need to take care of the message boundary problem. (Refer to this for an understanding of the problem : https://www.codeproject.com/Articles/11922/Solution-for-TCP-IP-
          client-socket-message-boundary)
        # Intraction with Secure_Information.txt file only done by Admin, so this increase security from extrnal thread.
        # we use mutex locking system/ipc for locking Specific task which are creating Ambiguities in crucial data like Account Balance.etc.
        # we mutex for safly perform tast by cuncurrent prorecesses, or use in inter communication with diffrent client.  

    
    -->> All Functionilty are display in cilent window, server window only display when new client are created , Total number are vistor is visited to this server today and lenght for Client Queue 
    Size.
    
    If you are User than its divide you in types - Normal user and Joint Account.
  
    If a User is not registered at Login Prompt, he/she is given an option to create a normal/joint account. An add query is generated to be approved by Admin.

    After a Normal user login succesfully than they provide following option to Perform Opertion like :

    Deposit : Deposit amount to your account.
    Withdraw : Withdraw some amount from your account.
    Balance Enquiry : Current balance in the account.
    Password Change : Generates a query for password change and Upload it in Admin_work.txt file for approval of Admin
    View Details : View details of user's account number specified by him/her.
    Delete Account : Generates a query for Deletion and Upload it in Admin_work.txt file for approval of Admin.
    Exit : Exit the menu.
    
    
    After a Joint user login succesfully than they provide following option to Perform Opertion like :
    
    -->>Key Feature: if deposite certain amount money or do any opeartion in an account so it reflection will have also saw in another account which are join to it.
    
    Deposit : Deposit amount to your account.  
    Withdraw : Withdraw some amount from your account.
    Balance Enquiry : Current balance in the account.
    Password Change : Generates a query for password change and Upload it in Admin_work.txt file for approval of Admin
    View Details : View details of user's account number specified by him/her.
    Delete Account : Generates a query for Deletion and Upload it in Admin_work.txt file for approval of Admin.
    Exit : Exit the menu.


    After an Admin logs in , the following menu is provided :
    
    Admin are play very Important Role for our system, beacuse it have authority to perform some crucial or important task on data base like Add account , Delete Account and Modify any account like 
    Wise Men say "WITH GREAT POWER COMES GREAT RESPONSIBILITY".
    -->>KEY FEATURES : We cannot creat Admin, Admin are whome which are only created by those which have a Direct access of in Persnol_information File for modification

    Execute pending Add queries
    Execute pending Delete queries.
    Execute pending Modify queries.
    Search for a specifc account details.
    Execute All Queries
    Exit


    The concepts applied throughout the project include Socket Programming , Process Management, File Management, Mutex Locking, Multithreading , Signal Handling and Inter Process Communication 
    Mechanisms.



3. Running the code

    To compile the server code, run the command : cc online_banking_server.c -lpthread
    To execute, run the command : ./a.out <port_number> (For eg : ./a.out 8000)
    To compile the client code, run the command : cc online_banking_client.c -lpthread
    To execute, run the command : ./a.out <port_number> (For eg : ./a.out 8000)

