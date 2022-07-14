
# MTX ARM OPERATING SYSTEM

This code was developed over the course of a semester while taking KC Wang (perhaps infamouse) CPTS-460 course. During the semester he takes you through his textbook [Design and Implementation of the MTX Operating System](https://link.springer.com/book/10.1007/978-3-319-17575-1) and has students implement their own variations of portions of his book, generally with some starter code. 

Prior to this assignment we worked on lower level operations like a boot loader, video driver, system memory management, and process switching and so this culminating assignment was adding the user space commands to interact with the system we built over the semester. For this assignment I implemented:

- `init` : the program that runs on process one, opening shells on tty0, ttyS0 and ttyS1 for login.
- `login` : A rudimentry password to log into the users shell. Multiple concurrent users is supported and it follow's the linux convetion of using `etc/passwd`
- `ls` : lists files using the long format, can be passed a path. 
- `cat` : When ran by itself echo stdin to stdout. If passed a path, it prints the contents to stdout.
- `more` : Scroll back was never implemented thus `more` serves as a paging tool 
- `grep` : Searches a file stream for a matching character set and prints the line. 
- `l2u` : Takes any incoming file stream and converts all chars to uppercase
- `cp` : Copies files in the os. 
- `>, >> and < IO Redirectionl Operations` : implemented for every command
- `| Pipes` : Can be used arbitrary amount of times  

This is an arm system that runs virtually inside qemu. If you'd like to run it, you'll need to run:
- `sudo apt install gcc-arm-none-eabi`
- `sudo apt install qemu-system-arm`


All the other code I wrote from my smester in his course can be found [here](https://github.com/SilasStokes/cpts-460-operating-systems)


## Tests to prove the functionality:

 Commands             Expected Results                 
-------------    --------------------------------    
1. Boot up     :  P1 (init) create logins         

2. login:      :  login from tty0                 

3.             :  login from ttyS0, ttyS1          

               :  logout; login again              

sh: Control-C  :  sh must not die                  

4. ls          :  SAME as ls -l in Linux,          
                  except no time field  
5. cat         : show input lines from stdin:     

6. cat f       : show contents of file f          

7. cat f1 > f2 
   ls          : f1, f2 MUST be same size         

8  grep abcd    :  grep for abcd from stdin

9. grep printf f: show lines with "printf" ONLY   

10. more f      : MUST pause after display ONE screen
                : Enter key: advance by line 
                : SPACE key: advance by screen    

11. cat f | more : MUST PAUSE for Enter/Space keys


12. cat f | grep printf : show lines with printf  


13. cat f | l2u | grep LINE:  2 pipes         


14. cat f | cat | l2u | grep LINE: 3 PIPEs 

15. cat < f | cat | grep print > out :    
    cat out (must see out contents)       

16. cp f g; ls :   show f and g SAME size  


