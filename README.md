# AquilaCodeDark



## AWS S3 Screenshots And videos folders (Syncing/Pull/Push to)

<details>
The Screenshots for this github and some videos folder is stored in s3 bucket
Pull down from bucket
  
```
  aws s3 cp --recursive s3://<github>/AquilaCodeDark/Videos AquilaCodeDark/Videos
  aws s3 cp --recursive s3://<github>/AquilaCodeDark/Assets AquilaCodeDark/Assets
  aws s3 cp --recursive s3://<github>/AquilaCodeDark/Screenshots AquilaCodeDark/Screenshots 
  aws s3 cp --recursive s3://<github>/AquilaCodeDark/Secrets AquilaCodeDark/Secrets 
  aws s3 cp --recursive s3://<github>/AquilaCodeDark/Tiled AquilaCodeDark/Tiled 
  aws s3 cp --recursive s3://<github>/AquilaCodeDark/assets AquilaCodeDark/src/assets 
```

Push to bucket
```
  aws s3 cp --recursive AquilaCodeDark/Videos s3://<github>/AquilaCodeDark/Videos
  aws s3 cp --recursive AquilaCodeDark/AllAssets s3://<github>/AquilaCodeDark/AllAssets
  aws s3 cp --recursive AquilaCodeDark/Screenshots s3://<github>/AquilaCodeDark/Screenshots
  aws s3 cp --recursive AquilaCodeDark/Secrets s3://<github>/AquilaCodeDark/Secrets
  aws s3 cp --recursive AquilaCodeDark/Tiled s3://<github>/AquilaCodeDark/Tiled
  aws s3 cp --recursive AquilaCodeDark/src/assets s3://<github>/AquilaCodeDark/assets
```

Or just do a sync
```
  aws s3 sync AquilaCodeDark/Videos s3://<github>/AquilaCodeDark/Videos --delete
  aws s3 sync AquilaCodeDark/Assets s3://<github>/AquilaCodeDark/Assets --delete
  aws s3 sync AquilaCodeDark/Screenshots s3://<github>/AquilaCodeDark/Screenshots --delete
  aws s3 sync AquilaCodeDark/Secrets s3://<github>/AquilaCodeDark/Secrets --delete  
  aws s3 sync AquilaCodeDark/Tiled s3://<github>/AquilaCodeDark/Tiled --delete  
  aws s3 sync AquilaCodeDark/src/assets s3://<github>/AquilaCodeDark/assets --delete  

```
</details>
