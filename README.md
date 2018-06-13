## neoRAD-IO2 C Libary


This libary is designed to work as a cmake subdirectory  
to use add the following to your CMakeLists.txt  
add_subdirectory(neoRAD-IO2-C-Library)  
...  
target_link_libraries(YourProjectName PUBLIC neoRADIO2)  

you can add this project as a git submodule   
git submodule add git@intrepidcs.homeip.net:emende/neoRAD-IO2-C-Library.git  
git submodule update --init --recursive  

