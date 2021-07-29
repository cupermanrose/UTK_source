if [  -d "/liblpsolve55.a" ]; then
	exit
fi
echo "You have not installed lp_solve library yet, downloading related files..."
rm lp_solve_5.5.2.0_source.tar.gz*
wget 'https://sourceforge.net/projects/lpsolve/files/lpsolve/5.5.2.0/lp_solve_5.5.2.0_source.tar.gz'
tar -xzvf lp_solve_5.5.2.0_source.tar.gz
rm lp_solve_5.5.2.0_source.tar.gz
cd ./lp_solve_5.5/lpsolve55
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     sh ccc;\
				mv ./bin/ux64/liblpsolve55.a ../../;\
				;;
    Darwin*)    sh ccc.osx;\
				mv ./bin/osx64/liblpsolve55.a ../../;;
    CYGWIN*)    .\cgcc.bat;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
cd ../..
rm -r ./lp_solve_5.5
rm ./lp_solve_5.5.2.0_source.tar.gz
clear
echo "finished downloading the lp_solve lib"
