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

---

## Deploy to AWS S3 + CloudFront (Static Site with HTTPS)

This documents deploying the Angular production build to S3, served via CloudFront with a custom domain and HTTPS.

### Prerequisites

- [AWS CLI v2](https://docs.aws.amazon.com/cli/latest/userguide/getting-started-install.html) installed
- AWS credentials configured (`aws configure`)
- A Route 53 hosted zone for your domain

### 1. Build for Production

```bash
npx ng build --configuration production
```

Output goes to `dist/aquila-code-dark/`.

### 2. Create S3 Bucket & Enable Static Hosting

```bash
aws s3 mb s3://<YOUR_DOMAIN>

aws s3 website s3://<YOUR_DOMAIN> --index-document index.html --error-document index.html
```

> Setting `--error-document index.html` ensures Angular client-side routing works (all routes fall back to index.html).

### 3. Upload the Build

```bash
aws s3 sync dist/aquila-code-dark/ s3://<YOUR_DOMAIN> --delete
```

### 4. Set Bucket Policy (Public Read)

```bash
aws s3api put-public-access-block --bucket <YOUR_DOMAIN> --public-access-block-configuration "BlockPublicAcls=false,IgnorePublicAcls=false,BlockPublicPolicy=false,RestrictPublicBuckets=false"
```

Create a file `bucket-policy.json`:
```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Sid": "PublicRead",
      "Effect": "Allow",
      "Principal": "*",
      "Action": "s3:GetObject",
      "Resource": "arn:aws:s3:::<YOUR_DOMAIN>/*"
    }
  ]
}
```

Apply it:
```bash
aws s3api put-bucket-policy --bucket <YOUR_DOMAIN> --policy file://bucket-policy.json
```

### 5. Request an SSL Certificate (ACM)

Must be in `us-east-1` for CloudFront:

```bash
aws acm request-certificate --domain-name <YOUR_DOMAIN> --subject-alternative-names "*.<YOUR_DOMAIN>" --validation-method DNS --region us-east-1
```

If your domain is registered through Route 53, DNS validation may complete automatically. Check status:

```bash
aws acm describe-certificate --certificate-arn <CERT_ARN> --region us-east-1 --query "Certificate.Status"
```

Wait until it returns `"ISSUED"` before proceeding.

### 6. Create CloudFront Distribution

Create a file `cf-config.json` (replace `<YOUR_DOMAIN>`, `<YOUR_REGION>`, and `<CERT_ARN>`):

```json
{
  "CallerReference": "unique-reference-string",
  "Aliases": {
    "Quantity": 1,
    "Items": ["<YOUR_DOMAIN>"]
  },
  "DefaultRootObject": "index.html",
  "Origins": {
    "Quantity": 1,
    "Items": [
      {
        "Id": "S3-origin",
        "DomainName": "<YOUR_DOMAIN>.s3-website-<YOUR_REGION>.amazonaws.com",
        "CustomOriginConfig": {
          "HTTPPort": 80,
          "HTTPSPort": 443,
          "OriginProtocolPolicy": "http-only"
        }
      }
    ]
  },
  "DefaultCacheBehavior": {
    "TargetOriginId": "S3-origin",
    "ViewerProtocolPolicy": "redirect-to-https",
    "AllowedMethods": {
      "Quantity": 2,
      "Items": ["GET", "HEAD"],
      "CachedMethods": {
        "Quantity": 2,
        "Items": ["GET", "HEAD"]
      }
    },
    "ForwardedValues": {
      "QueryString": false,
      "Cookies": { "Forward": "none" }
    },
    "MinTTL": 0,
    "DefaultTTL": 86400,
    "MaxTTL": 31536000,
    "Compress": true
  },
  "CustomErrorResponses": {
    "Quantity": 1,
    "Items": [
      {
        "ErrorCode": 404,
        "ResponsePagePath": "/index.html",
        "ResponseCode": "200",
        "ErrorCachingMinTTL": 0
      }
    ]
  },
  "ViewerCertificate": {
    "ACMCertificateArn": "<CERT_ARN>",
    "SSLSupportMethod": "sni-only",
    "MinimumProtocolVersion": "TLSv1.2_2021"
  },
  "Enabled": true,
  "Comment": "AquilaCode Dark",
  "PriceClass": "PriceClass_100"
}
```

Create the distribution:
```bash
aws cloudfront create-distribution --distribution-config file://cf-config.json
```

Note the `Distribution.DomainName` (e.g. `d1234abcd.cloudfront.net`) and `Distribution.Id` from the output.

### 7. Point Route 53 to CloudFront

Create a file `r53-config.json` (replace `<CLOUDFRONT_DOMAIN>`):

```json
{
  "Changes": [
    {
      "Action": "UPSERT",
      "ResourceRecordSet": {
        "Name": "<YOUR_DOMAIN>",
        "Type": "A",
        "AliasTarget": {
          "HostedZoneId": "Z2FDTNDATAQYW2",
          "DNSName": "<CLOUDFRONT_DOMAIN>",
          "EvaluateTargetHealth": false
        }
      }
    }
  ]
}
```

> `Z2FDTNDATAQYW2` is the fixed hosted zone ID for all CloudFront distributions.

Apply:
```bash
aws route53 change-resource-record-sets --hosted-zone-id <YOUR_HOSTED_ZONE_ID> --change-batch file://r53-config.json
```

### 8. Wait for Deployment

Check CloudFront status:
```bash
aws cloudfront get-distribution --id <DISTRIBUTION_ID> --query "Distribution.Status"
```

Once it returns `"Deployed"`, your site is live at `https://<YOUR_DOMAIN>`.

### Re-deploying

After code changes, rebuild and sync:

```bash
npx ng build --configuration production
aws s3 sync dist/aquila-code-dark/ s3://<YOUR_DOMAIN> --delete
aws cloudfront create-invalidation --distribution-id <DISTRIBUTION_ID> --paths "/*"
```

The invalidation clears CloudFront's cache so visitors see the new version immediately.
