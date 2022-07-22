# Sample program for testing FbsfApi
provided by Nicolas.Vasset@cea.fr

## Compilation

Use QtCreator or provided script buildTest.bat

The generated application FbsfApiTest is placed in ${FBSF_HOME}/lib/release

## Execution

*!!! Warning: this is an early development stage, you will need some manual config steps for tests to work properly*

The script runTest.bat provides a possible setup, using \${FBSF_HOME}/Tests/testMini

Some of the tests require using  ${FBSF_HOME}/Tests/testFMU_1 (need to adapt the script)


This will prompt for a command to run.


Here is the list of commands you can run depending on where you are:
* in testMini, tests t1, t1b, t2, t3 should work
* in testFMU_1, tests t2b, t4, t5 should work

if you are not in the correct path, you will get a message like

Error: configuration provided is invalid

ERROR: TestSimpleSimuFMU: 1-Initialization was not successful 

## Explanations on the testing procedure

To do other tests, you need to (these steps are done in the runTest.bat script):
1. include required Qt and FBSF libs (use QtVersion.bat and FbsfEnv.bat)
2. set QML_IMPORT_PATH (not sure it is required + should be in FsbfEnv.bat)
3. cd to running dir (e.g. testMini or testFMU_1)
4. set APP_HOME propertly (e.g. to the running dir)
5. start FbsfApiTest
6. write the test you want to run at the prompt
7. check it ends with SUCCESS

Unfortunately the tests can not be automated easily (with QtTest), apparently due 
to threading limitations. We will be working on it :-)