# Sample program for testing FbsfApi
provided by Nicolas.Vasset@cea.fr

## Compilation

Use QtCreator (launched through script ../environ.bat) in order to generate the application
The tests are validated in release mode and in full batch mode.
In particular they rely on applications which do not have a graphical dependency
If you wish to use the FbsfApi executable with xml applications with Graphical dependencies (DocLogic, DocGraphic...), 
you will need to compile in release mode. Failure to do so will result in a crash (Observable in FbsfFramework.log)

A dependency exists with ModuleMini for tests t1, t1b, t2, t3 (use of lightweight application TestMini/simul_NoGr.xml)

The generated application FbsfApiTest is placed in ${FBSF_HOME}/lib/release or ${FBSF_HOME}/lib/batch 

## Execution
Tests must be executed where the xml application is located.
Here for tests t1, t1b, t2, t3, this location is \${FBSF_HOME}/Tests/testMini
For tests t2b, t4, t5, the location is  ${FBSF_HOME}/Tests/testFMU

See provided scripts runTest.bat and runTestFMU.bat, or use QtCreator for testing

if you are not in the correct path, the initialization part of the test will fail, and the message obtained is:
Error: configuration provided is invalid

ERROR: TestSimpleSimuFMU: 1-Initialization was not successful 

A terminal can also be used here in order to test the HMI in an atomic way, see implementation of the main() class for testing

## Explanations on the testing procedure

To do other tests, you need to (these steps are done in the runTest.bat script):
1. include required Qt and FBSF libs (use QtVersion.bat and FbsfEnv.bat, possibly dependency on ModuleMini for use of the 
batch-compliant lightweight application TestMini/simul_NoGr.xml)
2. set QML_IMPORT_PATH (not sure it is required + should be in FsbfEnv.bat)
3. cd to running dir (e.g. testMini or testFMU_1)
4. set APP_HOME propertly (e.g. to the running dir)
5. start FbsfApiTest
6. write the test you want to run at the prompt
7. check it ends with SUCCESS

Unfortunately the tests can not be automated easily (with QtTest), apparently due 
to threading limitations. This will be under investigation