function checklib {
	sudo yum install qhull-devel lp-solve
	sudo apt-get install qhull-bin lpsolve
	brew install qhull lp_solve
}

checklib
cd UTK/src
make
