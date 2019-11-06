### HTTPServer
An HTTP server writen in C++ for high end embedded microcontrollers.
To build the example:
```
cd example/build
cmake ..
make
./http_server_example 8123 ../files
```
Todo:
- [ ] ADD HTTPServer namespace
- [ ] Sort route for longest match first
- [x] ADD HTML engine: [link](https://pantor.github.io/inja/)
- [x] Add View Controller
- [ ] Add Resource Controller
- [ ] Complete redirect function
