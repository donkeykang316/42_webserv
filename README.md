### qa
 qa process: 
   ```
   ./ubuntu_tester http://localhost:5000
   ```
 - `POST http://localhost:5000/ with a size of 0` causing the segmentfault (fixed)
 - Limit the client body (size: 1) (fixed)
 - ```
   curl -X POST http://localhost:5000 -H "Content-Type: text/plain" --data "BODY IS HERE write something shorter or longer than body limit"
   ```
   , the curl command hangs (fixed)
 - disable /www loction from the config file cause segmentfault
 - disable allow method post does not work (fixed)
 - multi server not working (fixed)
 - multi service config with the same port shouldnt work but work in our case
