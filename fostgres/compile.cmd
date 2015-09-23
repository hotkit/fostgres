@echo off
echo hello %*
pushd %0\..

call ..\boost-version.cmd

IF %BOOST_VERSION_MAJOR% LEQ 39 (
    ..\bjam preserve-test-targets=on %*
) ELSE (
    ..\bjam %*
)

IF ERRORLEVEL 1 (
    echo hello %* BUILD FAIL
    popd
    copy
) ELSE (
    popd
)
