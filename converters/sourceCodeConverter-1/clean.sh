
svn revert -R .
rm -f `find -name \*\.tmp\.\* -o -name \*\.org\*`
rm convert.log
rm localvariable.txt
touch convert.log
