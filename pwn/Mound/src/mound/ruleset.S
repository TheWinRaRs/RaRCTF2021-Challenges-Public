A = arch
A == ARCH_X86_64 ? next : dead
A = sys_number
A >= 0x40000000 ? dead : next
A == execve ? dead : next
A == execveat ? dead : next
A == open ? dead : next
A == close ? dead : next
A == creat ? dead : next
A == uselib ? dead : next
A == fork ? dead : next
A == vfork ? dead : next
A == clone ? dead : next
return ALLOW
dead:
return KILL