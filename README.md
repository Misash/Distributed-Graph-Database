# Distributed Graph Database

Database that store data distributed using TCP/IP protocol to connect beteween socket with the nodes that has the data , also you can make CRUD actions with a client , who writes commands , that then with a scanner will be categorasize and send to the server and nodes .

## Before  you start 


```bash
sudo apt -get-update -y
sudo apt-get install -y libsqlite3-dev
sudo apt -install sqlite3
```

    
## Documentation

[link to Documentation](https://www.overleaf.com/read/bqqvjvgxmmpd)


## Run Locally

Clone the project

```bash
  git clone git@github.com:Misash/Distributed-Graph-Database.git
```

Go to the project directory

```bash
  cd Distributed-Graph-Database
```
compile with Cmake

```bash
  mkdir build
  cd build/
  cmake ../
  cmake --build .
```

run the server with "n" nodes and one client

```bash
  ./server
  ./node
  ./client
```

## Features

| Action | command | 
| :---:   | :-: | 
| Create Node | ``` CN julio``` | 
| Create Attribute  |  ``` CA julio nationality peruvian```| 
|Create Relation |  ```  CN julio teacher Network```| 
| Read Attribute | ```  RA julio nationality``` | 
| Read Node at levels  |  ``` RN julio 5 ```| 
| Read Node Information |  ``` RI julio ```| 
| Update attribute | ``` RA julio nationality``` | 
| Update Node  |  ``` UN julio julius```| 
| Update Relation |  ``` UR julio teacher Network professor```| 
| Delete Attribute | ``` DA julio nationality``` | 
| Delete Node  |  ``` DN julio```| 
| Delete Relation |  ```DR julio teacher Network```| 


