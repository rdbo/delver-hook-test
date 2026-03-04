runasroot := 'doas'

inject-glibc: build-docker start_delver inject

build-docker:
	./docker-env.sh just build-dev

build-dev:
	cmake -B build . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cd build && make -j $(nproc)

clean:
	rm -rf build*

logs:
	@echo "===== STDOUT ====="
	@cat "/proc/$(pidof -s steam)/root/tmp/delverhook.out"
	@echo "===== STDERR ====="
	@cat "/proc/$(pidof -s steam)/root/tmp/delverhook.err"
	@echo "=================="

start_delver:
	delver_pid=$(just get_delver_pid 2>&1) && kill -9 $delver_pid || true
	pkill -9 delver || true
	xdg-open "steam://rungameid/249630"
	while ! just get_delver_pid >/dev/null 2>&1; do sleep 0.5; done

inject:
	{{runasroot}} cp "$(pwd)/libdelverhook.so" "/proc/$(pidof -s delver)/root/tmp/"
	{{runasroot}} injector -n delver "/tmp/libdelverhook.so"

get_delver_pid:
	@jps | grep delver.jar | awk '{print $1; exit} END {exit NR==0}'
