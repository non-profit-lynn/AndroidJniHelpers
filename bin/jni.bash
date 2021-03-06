#!/usr/bin/env bash
# Generates .jni file for class that was passed in
# className="" && filePath="" && clear && printf '\e[3J' && bin/jni.bash $className $filePath && cat bin/jni.files/generated/$className.jni
# className="" && filePath="" && clear && reset && bin/jni.bash $className $filePath && cat bin/jni.files/generated/$className.jni

: '

className="us.the.mac.android.jni.helpers.AndroidJniVolley"
filePath="/Users/christopher/git/the-mac/AndroidJni/AndroidJniHelpers/library/src/main/java/us/the/mac/android/jni/helpers/AndroidJniVolley.java"

className="us.the.mac.library.demo.androidjni.BasicRequests"
filePath="/Users/christopher/git/the-mac/AndroidJni/AndroidJniHelpers/demo/src/main/java/us/the/mac/library/demo/androidjni/BasicRequests.java"

clear && printf '\e[3J'
bin/jni.bash $className $filePath
cat bin/jni.files/generated/$className.jni && echo "" && echo ""
cat bin/jni.files/build.jni/$className.jniBlueprint


export className="us.the.mac.android.jni.helpers.MACVolleyRequests"
export filePath="/Users/christopher/git/the-mac/AndroidJni/AndroidJniHelpers/library/src/androidTest/java/us/the/mac/android/jni/helpers/MACVolleyRequests.java"
export jarFilePath="/Users/christopher/git/the-mac/VolleyExample/app/libs/volley.jar:/Users/christopher/git/the-mac/VolleyExample/app/libs/khandroid-httpclient-4.2.3.jar"

clear && printf '\e[3J'
bin/jni.bash $className $filePath
cat bin/jni.files/generated/$className.jni && echo "" && echo ""
cat bin/jni.files/build.jni/$className.jniBlueprint



export className="com.android.volley.Request.Method"
export className="com.android.volley.Request"
clear && printf '\e[3J'
bin/jni.bash $className && echo "" && echo ""
cat bin/jni.files/generated/$className.jni && echo "" && echo ""
cat bin/jni.files/build.jni/$className.jniBlueprint

'

debug=0

if [[ $debug == 1 ]] ; then
set -xe
fi

if [[ $@ == *--debug* ]] ; then
debugFlag="--debug"
fi

androidVersion=android-24

cp ../local.properties bin/jni.files
sdkPath=$(grep "^sdk.dir=" "bin/jni.files/local.properties" | cut -d'=' -f2)

	if [[ -z $sdkPath ]] ; then
echo "Result: $sdkPath was not found in the project"
echo "Check you project's local.properties file"
echo ""
	exit;
	fi

androidJarFile=$sdkPath/platforms/$androidVersion/android.jar
apacheJarFile=$sdkPath/platforms/$androidVersion/optional/org.apache.http.legacy.jar
volleyJarFile="$PWD/bin/jni.files/volley.jar"
khandroidJarFile="$PWD/bin/jni.files/khandroid-httpclient-4.2.3.jar"

	if [ ! -f $androidJarFile ] ; then
echo "Result: Android version $androidVerison was not found in the android sdk: $androidJarFile"
echo "Try: " $(grep $sdkPath/platforms)
echo ""
	exit;
	fi

className=${1##*.} #String

if [[ ! -z $jarFilePath ]] ; then  #####################################################################
	rm -rf bin/jni.files/build.jni
	mkdir bin/jni.files/build.jni
	cd bin/jni.files/build.jni
	packageName=${1//".$className"}. #java.lang
	filePath=${packageName//.//} #java/lang

    projectFolder=${PWD}/../../../..
    appJavaFolder=$projectFolder/app/src/main/java
    libraryJavaFolder=$projectFolder/AndroidJniHelpers/library/src/main/java

	mkdir -p $filePath && cp $2 $filePath$className.java
	javac $filePath$className.java -classpath $jarFilePath:$androidJarFile:$apacheJarFile -sourcepath $appJavaFolder:$libraryJavaFolder
	javap -v $packageName$className | grep -B 1 "descriptor" > "$1.jniBlueprint"

elif [[ $2 == *.java* ]] ; then  #####################################################################
	rm -rf bin/jni.files/build.jni
	mkdir bin/jni.files/build.jni
	cd bin/jni.files/build.jni
	packageName=${1//".$className"}. #java.lang
	filePath=${packageName//.//} #java/lang

    projectFolder=${PWD}/../../../..
    appJavaFolder=$projectFolder/app/src/main/java
    libraryJavaFolder=$projectFolder/AndroidJniHelpers/library/src/main/java

	mkdir -p $filePath && cp $2 $filePath$className.java
	javac $filePath$className.java -classpath $volleyJarFile:$khandroidJarFile:$androidJarFile:$apacheJarFile -sourcepath $appJavaFolder:$libraryJavaFolder
	javap -v $packageName$className | grep -B 1 "descriptor" > "$1.jniBlueprint"

elif [[ $1 == *.* ]] ; then  #####################################################################
	rm -rf bin/jni.files/build.jni
	mkdir bin/jni.files/build.jni
	cd bin/jni.files/build.jni
	packageName=${1//".$className"}. #java.lang
	filePath=${packageName//.//} #java/lang

	mkdir -p $filePath
	inputJarPath=$androidJarFile
	classFilePath=$(jar -tf $androidJarFile | grep -m 1 $className.class)

	if [[ -z $classFilePath ]] ; then
	inputJarPath=$apacheJarFile
	classFilePath=$(jar -tf $apacheJarFile | grep -m 1 $className.class)
	fi

	if [[ -z $classFilePath ]] ; then
	inputJarPath=$volleyJarFile
	classFilePath=$(jar -tf $volleyJarFile | grep -m 1 $className.class)
	fi

	if [[ -z $classFilePath ]] ; then
	inputJarPath=$khandroidJarFile
	classFilePath=$(jar -tf $khandroidJarFile | grep -m 1 $className.class)
	fi


	if [[ -z $classFilePath ]] ; then
echo "Result: $1 was not found in the android library or apache legacy"
echo "" && echo "Usage: bin/jni.bash FULLY_QUALIFIED_CLASS_NAME [OPTIONAL JAVA FILE PATH]"
echo "Try: bin/jni.bash $1 file/path/to/java/file/$className.java"
echo ""
	exit;
	fi

	unzip -p $inputJarPath $classFilePath > $filePath$className.class

	if [ ! -f $filePath$className.class ] ; then
echo "Result: $classFilePath was not found in the legacy jar: $apacheJarFile"
echo ""
	exit;
	fi

	javap -v $packageName$className | grep -B 1 "descriptor" > "$1.jniBlueprint"
else
echo "Usage: bin/jni.bash FULLY_QUALIFIED_CLASS_NAME [OPTIONAL JAVA FILE PATH]"
	exit;
fi

cp  ../../../library/src/helpers/java/GenerateJniHelpers.java .
cp ../jniDataTypes.properties .
cp ../jniReturnValues.properties .
cp -rf ../jniMethods .


javac GenerateJniHelpers.java

java GenerateJniHelpers "$1.jniBlueprint" "$debugFlag" > $1.jni
cp $1.jni ../generated/$1.jni

echo "Result: $className.jni has been generated and is ready to use."
echo "Path: ${PWD}/$1.jni"

cd ../..
# rm -rf bin/jni.files/build.jni