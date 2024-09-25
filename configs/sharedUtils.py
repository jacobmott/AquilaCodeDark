import shutil
import os
from rich import print
# Specify the source and destination paths

def configureEnvironment(environment):
  print("Configuring environment: " + environment);
  if environment == 'prod':
    configureProdEnvironment()
  elif environment == 'local':  #a.k.a. dev locally hosting everything on parent OS
    configureLocalEnvironment()
  elif environment == 'localDocker':  #a.k.a. dev locally but using docker
    configureLocalDockerEnvironment()    
  elif environment == 'thisisatest':  #just to test that things work
    configureThisIsATest()


def configureThisIsATest():
   copy_over("./testThisIsATest.ignorethisfile", "./test.ignorethisfile")

#a.k.a. dev
def configureLocalEnvironment():
   copy_over("./environment.developmentLOCAL.ts", "../angular-app/src/environments/environment.development.ts")   
   copy_over("./environmentLOCAL.ts", "../angular-app/src/environments/environment.ts")
   copy_over("./docker-composeLOCAL.yaml", "../docker-compose.yaml")
   copy_over("./nginxLOCAL.conf", "../nginx.conf")
   copy_over("../Secrets/envLOCALnest-app", "../nest-app/.env")
   copy_over("../Secrets/auth_configLOCALangular-app.json", "../angular-app/auth_config.json")
   copy_over("../Secrets/keysLOCALnest-app.ts", "../nest-app/src/config/keys.ts")


def configureProdEnvironment():
   copy_over("./environment.developmentPROD.ts", "../angular-app/src/environments/environment.development.ts")   
   copy_over("./environmentPROD.ts", "../angular-app/src/environments/environment.ts")
   copy_over("./docker-composePROD.yaml", "../docker-compose.yaml")
   copy_over("./nginxPROD.conf", "../nginx.conf")
   copy_over("../Secrets/envPRODnest-app", "../nest-app/.env")
   copy_over("../Secrets/auth_configPRODangular-app.json", "../angular-app/auth_config.json")
   copy_over("../Secrets/keysPRODnest-app.ts", "../nest-app/src/config/keys.ts")   


def configureLocalDockerEnvironment():
   copy_over("./environment.developmentLOCALDOCKER.ts", "../angular-app/src/environments/environment.development.ts")   
   copy_over("./environmentLOCALDOCKER.ts", "../angular-app/src/environments/environment.ts")
   copy_over("./docker-composeLOCALDOCKER.yaml", "../docker-compose.yaml")
   copy_over("./nginxLOCALDOCKER.conf", "../nginx.conf")
   copy_over("../Secrets/envLOCALDOCKERnest-app", "../nest-app/.env")
   copy_over("../Secrets/auth_configLOCALDOCKERangular-app.json", "../angular-app/auth_config.json")
   copy_over("../Secrets/keysLOCALDOCKERnest-app.ts", "../nest-app/src/config/keys.ts")      


# def copyLazy():
#    copy_over("./environment.developmentLOCAL.ts", "./environment.developmentLOCALDOCKER.ts")   
#    copy_over("./environmentLOCAL.ts", "./environmentLOCALDOCKER.ts")
#    copy_over("./docker-composeLOCAL.yaml", "./docker-composeLOCALDOCKER.yaml")
#    copy_over("./nginxLOCAL.conf", "./nginxLOCALDOCKER.conf")
#    copy_over("../Secrets/envLOCALnest-app", "../Secrets/envLOCALDOCKERnest-app")
#    copy_over("../Secrets/auth_configLOCALangular-app.json", "../Secrets/auth_configLOCALDOCKERangular-app.json")
#    copy_over("../Secrets/keysLOCALnest-app.ts", "../Secrets/keysLOCALDOCKERnest-app.ts")  


def copy_over(source_path, destination_path):
  print(f"{source_path} [yellow]copied[/yellow] over {destination_path}: ", end='')
  try:
    shutil.copy2(source_path, destination_path)
    print("[green](Success)[/green]")
  except FileNotFoundError:
    print("[red](Failure)[/red] : Source file not found.")
  except PermissionError:
    print("[red](Failure)[/red] : Permission denied.")
  except Exception as e:
    print(f"[red](Failure)[/red] : An error occurred: {e}")