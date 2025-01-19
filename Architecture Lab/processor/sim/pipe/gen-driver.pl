#!/usr/bin/perl 
#!/usr/local/bin/perl 

#
# gen-driver - Generate driver file for any to_binary_string function
#
use Getopt::Std;

$n = 0;

getopts('hcrn:f:b:');

if ($opt_h) {
    print STDERR "Usage $argv[0] [-h] [-c] [-n N] [-f FILE]\n";
    print STDERR "   -h      print help message\n";
    print STDERR "   -c      include correctness checking code\n";
    print STDERR "   -n N    set number of elements\n";
    print STDERR "   -f FILE set input file (default stdin)\n";
    print STDERR "   -b blim set byte limit for function\n";
#    print STDERR "   -r      Allow random result\n";
    die "\n";
}

$check = 0;
if ($opt_c) {
    $check = 1;
}

$bytelim = 1000;
if ($opt_b) {
    $bytelim = $opt_b;
}

if ($opt_n) {
    $n = $opt_n;
    if ($n < 0) {
	print STDERR "n must be at least 0\n";
	die "\n";
    }
}

#$randomval = 0;
# Accumulated count
$rval = 0;

#if ($opt_r) {
#  $randomval = 1;
#} else {
# Value that should be returned by function
#  $tval = int($n/2);
#}


# The data to be stored.
@data = ();

for ($i = 0; $i < $n; $i++) {
  $data[$i] = ($i+1);
  $rval+=$data[$i];
  #if ($randomval) {
  # if (int(rand(2)) == 1) {
  #   $data[$i] = -$data[$i];
  #   $rval++;
  # }
  #} else {
  #  if ($rval < $tval && int(rand(2)) % 2 == 1 ||
  #	$tval - $rval >= $n - $i) {
  #    $data[$i] = -$data[$i];
  #    $rval++;
  #  }
  #}
}


# Values to put at beginning and end of destination
$Preval =  "0xbcdefa";
$Postval = "0xdefabc";
$Corrval = "\$0x5710331";

print <<PROLOGUE;
#######################################################################
# Test for copying block of size $n;
#######################################################################
	.pos 0
main:	irmovq Stack, %rsp  	# Set up stack pointer

	# Set up arguments for copy function and then invoke it
	irmovq \$$n, %rdx		# src and dst have $n elements
	irmovq dest, %rsi	# dst array
	irmovq src, %rdi	# src array
    # corrupt all the unused registers to prevent assumptions
    irmovq $Corrval, %rax
    irmovq $Corrval, %rbx
    irmovq $Corrval, %rcx
    irmovq $Corrval, %rbp
    irmovq $Corrval, %r8
    irmovq $Corrval, %r9
    irmovq $Corrval, %r10
    irmovq $Corrval, %r11
    irmovq $Corrval, %r12
    irmovq $Corrval, %r13
    irmovq $Corrval, %r14
	call to_binary_string		
	 
PROLOGUE

if ($check) {
print <<CALL;
	call check	        # Call checker code
	halt                # should halt with 0xaaaa in %rax
.pos 200
temp_data:
 .quad 0
CALL
} else {
print <<HALT;
	halt			# should halt with sum in %rax
HALT
}

print "StartFun:\n";
if ($opt_f) {
    open (CODEFILE, "$opt_f") || die "Can't open code file $opt_f\n";
    while (<CODEFILE>) {
	printf "%s", $_;
    }
} else {
    while (<>) {
	printf "%s", $_;
    }
}
print "EndFun:\n";

if ($check) {
$len_min_1 = 8 * $n - 8;
print <<CHECK;
#################################################################### 
# Epilogue code for the correctness testing driver
####################################################################

# This is the correctness checking code.
# It checks:
#   1. %rax has $rval.  Set %rax to 0xbbbb if not.
#   2. The total length of the code is less than or equal to $bytelim.
#      Set %rax to 0xcccc if not.
#   3. The source data was converted to string and stored in the destination.
#      Set %rax to 0xdddd if not.
#   4. The words just before and just after the destination region
#      were not corrupted.  Set %rax to 0xeeee if not.
# If all checks pass, then sets %rax to 0xaaaa
check:
	# Return value test
	irmovq \$$rval,%r10
	subq %r10,%rax
	je checkb
	irmovq \$0xbbbb,%rax  # Failed test #1
	jmp cdone
checkb:
	# Code length check
	irmovq EndFun,%rax
	irmovq StartFun,%rdx
	subq %rdx,%rax
	irmovq \$$bytelim,%rdx
	subq %rax,%rdx
	jge checkm
	irmovq \$0xcccc,%rax  # Failed test #2
	jmp cdone
checkm:
	irmovq dest, %rdx     # Pointer to next destination location
	irmovq src, %rbx      # Pointer to next source location
	irmovq \$$n,%rdi      # Count
	andq %rdi,%rdi
	je checkpre           # Skip check if count = 0
mcloop:
	irmovq  temp_data, %r10       # temp_data
	mrmovq (%rbx),%rsi
	irmovq \$128, %rcx       # pow = 128
L1:
        andq %rcx,%rcx    	 # pow <= 0?
        jle mcheck	
	rrmovq %rcx, %r8
        subq %rsi, %r8
        jg L2                # check if val >= pow
        subq %rcx, %rsi        # val -= pow;
        irmovq \$49, %r8         
        rmmovq %r8, (%r10)     # *temp = '1'
        jmp L3
L2:   
        irmovq \$48, %r8
        rmmovq %r8, (%r10)     # *temp = '0'
L3:  
        irmovq \$1, %r8
        addq %r8, %r10         # temp++
  
        # all this for pow>>=1
        xorq %r11, %r11          
L4:
        rrmovq %r8, %r12
        subq %rcx, %r12
        je L5
    
        rrmovq %r8, %r11
        addq %r8, %r8
        jmp L4  
L5:
        rrmovq %r11, %rcx
        jmp L1
mcheck:
        irmovq temp_data, %r10
        mrmovq (%r10), %rax
        mrmovq (%rdx), %rsi
        subq %rsi, %rax  
	je  mok
	irmovq \$0xdddd,%rax  # Failed test #3
	jmp cdone
mok:
	irmovq \$8,%rax
	addq %rax,%rdx	      # dest++
	addq %rax,%rbx       # src++
	irmovq temp_data, %r10
	irmovq \$0, %rax
	rmmovq %rax, (%r10)   #temp_data = 0
	irmovq \$1,%rax
	subq %rax,%rdi       # cnt--
	jg mcloop
checkpre:
	# Check for corruption
	irmovq Predest,%rdx
	mrmovq (%rdx), %rax  # Get word before destination
	irmovq \$$Preval, %rdx
	subq %rdx,%rax
	je checkpost
	irmovq \$0xeeee,%rax  # Failed test #4
	jmp cdone
checkpost:
	# Check for corruption
	#irmovq Postdest,%rdx
	#mrmovq (%rdx), %rax  # Get word after destination
	#irmovq \$$Postval, %rdx
	#subq %rdx,%rax
	#je checkok
	#irmovq \$0xeeee,%rax # Failed test #4
	#jmp cdone
checkok:
	# Successful checks
	irmovq \$0xaaaa,%rax
cdone:
	ret
CHECK
}

print <<EPILOGUE1;

###############################
# Source and destination blocks 
###############################
	.align 8
src:
EPILOGUE1

for ($i = 0; $i < $n; $i++) {
    print "\t.quad $data[$i]\n";
}

print <<EPILOGUE2;
	.quad $Preval # This shouldn't get moved

	.align 16
Predest:
	.quad $Preval
dest:
EPILOGUE2

for ($i = 0; $i < $n; $i++) {
    print "\t.quad 0xcdefab\n";
}

print <<EPILOGUE3;
Postdest:
	.quad $Postval

.align 8
# Run time stack
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0
	.quad 0

Stack:
EPILOGUE3

