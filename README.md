# AIM Server

The AIM server provides a REST interface to the aimtools and hence the AI modules themselves.

    nodejs server.js

And in a different console

    curl --request GET http://localhost:3000/modules
    curl --request POST http://localhost:3000/modules/WriteModule/0
    curl --request DELETE http://localhost:3000/modules/WriteModule/0

The GET request shows the modules that are available. The POST request starts a new module with the name "WriteModule" and the identifier "0". The DELETE request stops an already running module, in this case the "WriteModule" again. The server will have authentication as user identifiers next to be able to track the modules on the server. There is some sanitizing going on with respect to the name of the module and the identifier. However, in the end, the idea is to have this thing running as a virtual machine, preferably one per user. Virtual machines become cheaper and cheaper over time and if you have a VM per user the fun in accessing this temporary VM diminish quickly.

## Copyrights
The copyrights (2013) belong to:

- License: LGPL v.3
- Author: Anne van Rossum
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands

