mkdir -p ${BUILD_DIR}

CMAKE_ARGS=(
    -DCMAKE_TOOLCHAIN_FILE="/scripts/${PLATFORM}.cmake"
    -DCONAN_DISABLE_CHECK_COMPILER=1
    -DBINDIR:PATH=./
    -DLIBDIR:PATH=./
    -DDATADIR:PATH=./
    -DMARCH_FLAG="${MYARCHTUNE}"
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
    -DCMAKE_C_COMPILER_LAUNCHER=ccache
    -DCMAKE_CXX_FLAGS="${MYCFLAGS}"
    -DCMAKE_C_FLAGS="${MYCFLAGS}"
    -DCMAKE_CXX_FLAGS_${MYBUILDTYPE}="${MYBUILDTYPEFLAGS}"
    -DCMAKE_C_FLAGS_${MYBUILDTYPE}="${MYBUILDTYPEFLAGS}"
    -DCMAKE_BUILD_TYPE="${MYBUILDTYPE}"
    -DAI_TYPES=NATIVE
    -DCMAKE_INSTALL_PREFIX:PATH="${INSTALL_DIR}"
    -DUSERDOCS_PLAIN=ON
    -DINSTALL_PORTABLE=ON
    -DWITH_MAPCOMPILER=OFF
    -DAI_EXCLUDE_REGEX="^CppTestAI$"
    -DMINGWLIBS="${SPRING_DIR}/mingwlibs64"
    -G Ninja
)

if [ "${PLATFORM}" == "linux-64" ]; then
    CMAKE_ARGS+=(
        -DBUNDLED_LIBC=1
    )
fi

if [ ! -z "${BUILD_CONFIG}" ]; then
    for BCONFIG in legacy headless dedicated; do
        if [ "${BCONFIG}" == "${BUILD_CONFIG}" ]; then
            CMAKE_ARGS+=(-DBUILD_spring-${BCONFIG}=TRUE)
        else
            CMAKE_ARGS+=(-DBUILD_spring-${BCONFIG}=FALSE)
        fi
    done
fi

cd "${BUILD_DIR}"
cmake "${CMAKE_ARGS[@]}" "${SPRING_DIR}"
