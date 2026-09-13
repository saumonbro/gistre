#/usr/bin/env bash


STEP=1
# test is used with a redirection to 3 so that every openat gets 3 and still manages to do its work
echo "Some dummy data" > test

if [ "$#" -eq 1 ]; then
	STEP="$1"
fi

# inject=sync:retval=0 	       ignore every call to sync
# inject=openat:retval=3       every openat returns 3 so it uses the dummy file we just created
# inject=close:retval=0        pretend close doesnt completly fail each time 
# inject=kill:retval=1         skip the call to kill so we can continue
# inject=read:retval=493       pretend that read returned 493 to skip the exit 1

# Step 1

if [ "$STEP" -eq 1 ]; then
	strace -f -e inject=openat:retval=3 -e inject=read:retval=493 -e inject=kill:retval=1 -e inject=sync:retval=0 -e inject=close:retval=0 ./straceme 3< test
fi

# inject=uname:retval=1   skip the step 1 ending 

# Step 2
if [ "$STEP" -eq 2 ]; then
	sudo -v
	sudo socat -v tcp-l:80, SYSTEM:"echo pong" &
	sleep 2
	# Since there will be some kind of strcmp over a fixed size (4 ?) we must not replace the last readl return value with 493. The when 2 remains arbitrary, it sometimes is 4 or 5.
	strace -f -e inject=openat:retval=3 -e inject=uname:retval=1 -e inject=read:retval=493:when=2 -e inject=kill:retval=1 -e inject=sync:retval=0  -e inject=close:retval=0 ./straceme 3< test

fi
