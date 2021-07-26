$i=0; Get-Content train.csv -ReadCount 250000 | %{$i++; $j=$i*250; $_ | Out-File $("train_"+$j+"k.csv")}

# train.csv is the input file
# 250000 is the number of lines per output file
# "train_"+$j+"k.csv" is the output name scheme with $j being a variable number