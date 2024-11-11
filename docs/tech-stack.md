# FIO.Relic Tech Stack

### The What
The FIO.Relic tech stack is the set of technologies used to develop and/or deploy the application, including the programming languages, frameworks, databases, front-end and back-end tools, and APIs. While the production and development tech stack will be very similar, the development environment is standalone, and may have a slightly different stack/deployment then a typical production envirionment. For example, a temporary PostgreSQL instance might be stood up and deployed using docker for development and/or testing purposes. Refer to the [Chronicle Setup](https://github.com/EOSChronicleProject/chronicle-tutorial/blob/master/02_chronicle_setup.md) document for further details.

#### Server Spec
* Hardware:
  * ubuntu-focal-20.04-amd64-server
  * Storage: 120GB minimum
  * Physical CPU: >= 2
  * Virtual CPU: >= 4
  * Architecture: x86_64
  * Memory: >= 12GB
  * Clock Speed: 2.2 GHz

* OS: Ubuntu 20.04

* Software
  * Programming Language: C++
  * Bash/Python
 
* Database
  * PostgreSQL 16

## Production API Interaction
```mermaid
 graph TD
     A[FIO Nodeos] --> B[FIO.Relic]
     B <--> C[Client Node 01]
     B <--> D[Client Node 02]
     B <--> ...
     B <--> E[Client Node N]
```

## FIO.Relic System Architecture
```mermaid
    C4Context
      title System Context diagram for FIO Relic incl FIO Nodeos, FIO Chronicle, and External API System
      Enterprise_Boundary(b0, "FIO Relic EcoSystem") {
        Person(customerA, "Block Producer", "Capturing V? History")
        System_Ext(SystemD, "External FIO Relic Client", "Uses FIO Chronicle Data.")

        Enterprise_Boundary(b1, "FIO.Relic") {

          System(SystemA, "FIO.Chroicle", "The FIO.Chronicle History Application")
          SystemDb(SystemB, "FIO.Chronicle Database", "Relational History Database")

        }
      }

      Rel(SystemA, customerA, "Uses", "History API")
      BiRel(SystemA, SystemB, "Uses")
      BiRel(SystemA, SystemD, "Uses")
      UpdateElementStyle(customerA, $fontColor="red", $bgColor="grey", $borderColor="red")
      UpdateRelStyle(SystemA, customerA, $textColor="red", $lineColor="red", $offsetX="-50", $offsetY="20")

      UpdateLayoutConfig($c4ShapeInRow="3", $c4BoundaryInRow="1")
```

### Example Sequence Diagram; once the logic is complete this will outline general sequence of historical data flow
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

