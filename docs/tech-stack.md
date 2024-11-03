# FIO.Relic Tech Stack

### The What
The FIO.Relic tech stack is the set of technologies used to develop and/or deploy the application, including the programming languages, frameworks, databases, front-end and back-end tools, and APIs. As the FIO Development Envirionment is standalone, it will have a slightly different deployment then a typical production envirionment.

#### Server Spec
* Hardware:
  * ubuntu-focal-20.04-amd64-server
  * Storage: 120GB minimum
  * Physical CPU: >= 2
  * Virtual CPU: >= 4
  * Architecture: x86_64
  * Memory: >= 8GB
  * Clock Speed: 2.2 GHz

* OS: Ubuntu 20.04

* Software
  * Programming Language: C++
  * Bash/Python
 
* Database
  * PostgreSQL 16

## Simple Production API Interaction
```mermaid
 graph TD
     A[FIO Nodeos] <--> B[FIO.Relic]
     B --> C[Server01]
     B --> D[Server02]
     B --> E[Server03]
     B --> ...
```

## FIO System Architecture
```mermaid
    C4Context
      title System Context diagram for FIO Nodeos, Chronicle, External API System
      Enterprise_Boundary(b0, "FIO EcoSystem") {
        Person(customerA, "Block Producer", "Capturing V? History")
        
        Enterprise_Boundary(b1, "FIO.Relic") {

          System(SystemC, "FIO.Chroicle", "The FIO.Chronicle History Application")
          SystemDb(SystemD, "FIO.Chronicle Database", "Relational History Database")

        }
      }

      Rel(SystemC, customerA, "Uses", "History API")
      UpdateElementStyle(customerA, $fontColor="red", $bgColor="grey", $borderColor="red")
      UpdateRelStyle(SystemC, customerA, $textColor="red", $lineColor="red", $offsetX="-50", $offsetY="20")

      UpdateLayoutConfig($c4ShapeInRow="3", $c4BoundaryInRow="1")
```

### OLD
### Example Architecture Diagram; to aid in creation of a tech stack diagram
```mermaid
    architecture-beta
        group api(cloud)[API]
    
        service db(database)[Database] in api
        service disk1(disk)[Storage] in api
        service disk2(disk)[Storage] in api
        service server(server)[Server] in api
    
        db:L -- R:server
        disk1:T -- B:server
        disk2:T -- B:db
```

```mermaid
sequenceDiagram
    participant dotcom
    participant iframe
    participant viewscreen
    dotcom->>iframe: loads html w/ iframe url
    iframe->>viewscreen: request template
    viewscreen->>iframe: html & javascript
    iframe->>dotcom: iframe ready
    dotcom->>iframe: set mermaid data on iframe
    iframe->>iframe: render mermaid
```

Front End Framework
   Client Side

