#!/bin/bash
premake5 gmake && ../../Utils/compiledb-generator/compiledb-gen-make.sh -C ../build/ config=optdebug_native -- -o ../../compile_commands.json
