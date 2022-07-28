Le dossier TEST-FBSF inclu les test suivants :

	TestSimu.xml 		: test en mode normal
	TestSimuMPC.xml 	: test en mode MPC
	TestBatch.xml 		: test en mode batch full

Pour executer les tests :

	Creer une session QTCreator nommée TEST-FBSF-TRUNK avec :
		Le projet FBSF.pro
		Le projet TEST-FBSF.pro
	
	Executer le fichier environ.bat
	
Dans le QtCreator :

	Supprimer les shadow build
	definir une dependance TEST-FBSF vers FBSF (sans Synchronize configuration)

	Pour le projet FBSF :
		ajouter une config de build Batch avec :
			Additional arguments : CONFIG+=BATCH
	
	Pour le projet TEST-FBSF declarer les config de run comme suit :
	
		Test Simu
		Executable : %FBSF_HOME%\lib\%{BuildConfig:Name}\FbsfFramework.exe
		Command line arguments : -no-logfile TestSimu.xml
		Working directory : %{buildDir}		
		
		Test SimuMPC
		Executable : %FBSF_HOME%\lib\%{BuildConfig:Name}\FbsfFramework.exe
		Command line arguments : -no-logfile TestSimuMPC.xml
		Working directory : %{buildDir}
		
		Test Batch
		Executable : %FBSF_HOME%\lib\batch\FbsfBatch.exe
		Command line arguments : -no-logfile TestBatch.xml
		Working directory : %{buildDir}