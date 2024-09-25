

# Git ignore files
Adding all our secrets and config files to gitignore since we never should change them
They are generated now via the config scripts in configs directory and saved in other places
Cant ignore these since we want the shells/placeholder files commited
documenting this here and we are usng skip-worktree to ignore these but keep a default placeholder 
version commited
https://stackoverflow.com/questions/13630849/git-difference-between-assume-unchanged-and-skip-worktree/13631525#13631525
git update-index --skip-worktree <file_name>
angular-app/auth_config.json
angular-app/src/environments/environment.development.ts
angular-app/src/environments/environment.ts
nest-app/.env
nest-app/src/config/keys.ts
nginx.conf


#Config file locations
```
[5.2][592][jacob@jakesbeastmech][/d/OtherProjects/AquilaCode/Secrets]
$ls
total 494K
-rw-r--r-- 1 jacob 197609  602 Aug 17 16:19 auth_configLOCALangular-app.json
-rw-r--r-- 1 jacob 197609  603 Aug 15 03:21 auth_configPRODangular-app.json
-rw-r--r-- 1 jacob 197609  185 Aug 17 15:03 envLOCALnest-app
-rw-r--r-- 1 jacob 197609  184 Aug 13 21:47 envPRODnest-app
-rw-r--r-- 1 jacob 197609 485K Aug 13 22:39 how_to_build_terminal_session.txt
-rw-r--r-- 1 jacob 197609  401 Aug 13 21:49 keysLOCALnest-app.ts
-rw-r--r-- 1 jacob 197609  419 Aug 17 16:27 keysPRODnest-app.ts
```

```
[5.2][597][jacob@jakesbeastmech][/d/OtherProjects/AquilaCode/configs]
$ls -lt | egrep 'LOCAL|PROD'
-rw-r--r-- 1 jacob 197609 1.5K Aug 17 17:27 docker-composeLOCAL.yaml
-rw-r--r-- 1 jacob 197609 1.5K Aug 17 17:26 docker-composePROD.yaml
-rw-r--r-- 1 jacob 197609 3.0K Aug 17 17:11 nginxLOCAL.conf
-rw-r--r-- 1 jacob 197609 3.0K Aug 17 15:05 nginxPROD.conf
-rw-r--r-- 1 jacob 197609  743 Aug 17 17:10 environmentPROD.ts
-rw-r--r-- 1 jacob 197609  744 Aug 17 17:10 environmentLOCAL.ts
-rw-r--r-- 1 jacob 197609  736 Aug 17 15:01 environment.developmentLOCAL.ts
-rw-r--r-- 1 jacob 197609  743 Aug 17 15:01 environment.developmentPROD.ts

```