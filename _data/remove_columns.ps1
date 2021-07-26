For ($i = 500; $i -le 3000; $i+=250)
{
	$source = "train_"+$i+"k.csv";
	$dest = "train_"+$i+"k_clean.txt"

(Import-CSV $source -Header 1,2,3 | Select "3" | ConvertTo-Csv -NoTypeInformation | Select-Object -Skip 1) -replace '"' | Set-Content $dest
}

# 1,2,3 are column numbers
# Select "3" is pulling the 3rd column in