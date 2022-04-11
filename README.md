# LibreSpeedCBackend
Thanks for using my LibreSpeed C Backend project! It is born as a separate backend for [LibreSpeed](https://librespeed.org), a speed test created and maintened by [Federico Dossena](https://fdossena.com).
It has been developed as an University project. If you want to take a look at the official document I presented to my teacher, please click here.

# Local experience

The server has been developed for Linux-based distros. If you want to compile the program, you can do it using this command directly in the root of the project:

gcc -w -o server server.c -pthread

The -pthread parameter is mandatory since pthread is part of the program. The option -w is optional and it is used to suppress some output messages that I will resolve with the next releases of the program.
Sometimes could be useful also to load symbols just to facilitate debugging phases: if you want to enable symbols, run this command:

gcc -g -w -o server server.c -pthread

# HTTPS

The server currently natively supports HTTP requests. In order to make the server working with HTTPS requests too, it will be necessary to setup a reverse proxy to route and serve secure requests. More information available in the project report [available at this link](https://speedtest.sergiomeloni.com/report.pdf).

# Other info
Please note that this is an academic project and it will be improved in the future. Other information about the project, speed tests, and documentation available [in the official report of the project](https://speedtest.sergiomeloni.com/report.pdf).
