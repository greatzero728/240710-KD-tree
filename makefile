VCPKG-PATH=/vcpkg/scripts/buildsystems/vcpkg.cmake
BUILD-TYPE=Release
TEST-EXE=bin/cs350-test

.PHONY: build clean check-warnings memcheck static-analysis

check-warnings: clean
	cmake -Bbuild ./ -DCMAKE_BUILD_TYPE=$(BUILD-TYPE) -DCMAKE_TOOLCHAIN_FILE=$(VCPKG-PATH) -DCMAKE_COMPILE_WARNING_AS_ERROR=1
	cmake --build ./build --target cs350-test

build: clean
	cmake -Bbuild ./ -DCMAKE_BUILD_TYPE=$(BUILD-TYPE) -DCMAKE_TOOLCHAIN_FILE=$(VCPKG-PATH)
	cmake --build ./build --target cs350-test
	
memcheck: 
	valgrind --quiet --leak-check=full --leak-resolution=med --track-origins=yes --error-exitcode=-1 --vgdb=no $(TEST-EXE) --gtest_filter=*
	@# No leaks but invalid accesses are detected
	@#valgrind --quiet --leak-check=no --leak-resolution=med --track-origins=yes --error-exitcode=-1 --vgdb=no $(TEST-EXE) --gtest_filter=*

static-analysis:
	cmake -Bbuild ./ -DCMAKE_BUILD_TYPE=$(BUILD-TYPE) -DCMAKE_TOOLCHAIN_FILE=$(VCPKG-PATH)
	cmake --build ./build --target clang-tidy

clean:
	rm -rf build/
