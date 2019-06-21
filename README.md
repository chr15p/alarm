# alarm
a little program to send signals after a time period in rust and c as a learning experience

The original alarm.c was soemthing I wrote 10+ years ago when I was learning c and discovered again just as I was starting to learn rust. Please excuse the slightly dodgy code :)

The utility forks off a process that sits in the background and then, after a given period of time sends a signal to a process, this is mainly for adding timeouts to shell scripts (so it will kill a command after 30 seconds for example)

to compile the c version:
```
gcc alarm.c -o alarm
```


to build the rust version
```
rust build
```

use -h to get the full set of options for each version (they are slightly different)
