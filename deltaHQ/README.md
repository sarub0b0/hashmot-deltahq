# Modification
Sequential -> Parallel Distributed

- Progress  
Parallel

- TODO  
Distributed

# Require

- OS  
Ubuntu 12.04, 14.04, 16.04  
openSUSE Leap 42.3  
Gentoo Linux  

- Packages:  
gcc, clang, libexpat

# Compile
```
$ make
```

# DeltaQ Usage
- Scenario create
```
$ ./deltaQ/deltaQ scenario/scenario.xml
```

- Text to binary
```
$ ./bin/scenario\_converter -i scenario/scenario.xml.out -o scenario/scenario.xml.bin
```

# Meteor Usage
- Start meteor
```
# ./bin/meteor -q scenario/scenario.xml.out -s scenario/scenario.xml.settings -d in -i 0 -I eth0
```
