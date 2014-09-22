build() {
	echo "#ifndef __VERSION_H__" > base/version.h
	echo "#define __VERSION_H__" >> base/version.h
	echo "#define VERSION \"$1\"" >> base/version.h
	echo "#endif" >> base/version.h

	cd base
	make
	cd ../login_server
	make
	cd ../route_server
	make
	cd ../msg_server
	make
    cd ../msfs
    make
	#cd ../file_server
	#make
	#cd ../test_client
	#make
	#cd ../tools
	#make

	cd ../

	#copy executables to run/ dir
	cp login_server/login_server ../run/login_server/
	cp route_server/route_server ../run/route_server/
	cp msg_server/msg_server ../run/msg_server/
    cp msfs/msfs ../run/msfs/
	cp file_server/file_server ../run/file_server/
	cp tools/daeml ../run/

	build_name=im-server-$1.tar.gz
	if [ -e "$build_name" ]; then 
		rm $build_name
	fi
	tar zcvf $build_name login_server/login_server route_server/route_server msg_server/msg_server msfs/msfs
        # file_server/file_server
}

clean() {
	cd base
	make clean
	cd ../login_server
	make clean
	cd ../route_server
	make clean
	cd ../msg_server
	make clean
    cd ../msfs
    make clean
	cd ../file_server
	make clean
	cd ../test_client
	make clean
}

print_help() {
	echo "Usage: "
	echo "  $0 clean --- clean all build"
	echo "  $0 version version_str --- build a version"
}

case $1 in
	clean)
		echo "clean all build..."
		clean
		;;
	version)
		if [ $# != 2 ]; then 
			echo $#
			print_help
			exit
		fi

		echo $2
		echo "build..."
		build $2
		;;
	*)
		print_help
		;;
esac
