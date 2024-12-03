### qa
 qa process: 
   ```
   ./ubuntu_tester http://localhost:5000
   ```
 - `POST http://localhost:5000/ with a size of 0` causing the segmentfault
 - Limit the client body (size: 1)
 - ```
   curl -X POST http://localhost:5000 -H "Content-Type: text/plain" --data "BODY IS HERE write something shorter or longer than body limit"
   ```
   , the curl command hangs
 - disable /www loction from the config file cause segmentfault
 - disable allow method post does not work
 - multi server not working
 - multi service config with the same port shouldnt work but work in our case
