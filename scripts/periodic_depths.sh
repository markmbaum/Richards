#settings file to use
fnset="../settings.txt"
#depths to use
depths="1 2 3 4 5 6 7 8 9 10 12 14 16 18 20 25 30 50"
#top output batch directory
dirbat="/n/scratchlfs/wordsworth_lab/markbaum/richards/batch"

#check if top batch directory exists, remaking it if so
if [ -d $dirbat ]; then
  rm -r $dirbat
fi
mkdir $dirbat

#copy the settings file to the top output dir
cp $fnset $dirbat
fnset=${dirbat}/$(basename $fnset)
echo $fnset

#loop through domain depths
for depth in $depths; do
  #make the output directory
  dirout=${dirbat}/${depth}
  mkdir $dirout
  #start a batch run
  #echo $fnset
  #echo $depth
  #echo $dirout
  sbatch periodic.batch $fnset $depth $dirout
done
