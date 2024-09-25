import { ActivatedRouteSnapshot, DetachedRouteHandle } from "@angular/router";
import { BaseRouteReuseStrategy } from "./base-route-reuse-strategy.service";

export class CustomRouteReuseStrategy extends BaseRouteReuseStrategy {
  private storedRoutes = new Map<string, DetachedRouteHandle>();

  // override shouldDetach(route: ActivatedRouteSnapshot): boolean {
  //   console.log("Called CustomRouteReuseStrategy: shouldDetach");
  //   const path: string = this.getPath(route);
  //   return path === "ships" || path === "play";
  // }

  // getPath(route: ActivatedRouteSnapshot): string {
  //   let path: string = "";
  //   console.dir(route);
  //   if (route.routeConfig!.path) {
  //     return route.routeConfig!.path;
  //   } else if (route.routeConfig!.component!.name === "_ShipListComponent") {
  //     path = "ships";
  //   } else if (route.routeConfig!.component!.name === "_PlayComponent") {
  //     path = "play";
  //   } else if (route.routeConfig!.component!.name === "_HomeComponent") {
  //     path = "home";
  //   }
  //   console.log("getPath: " + path);
  //   return path;
  // }

  // override store(
  //   route: ActivatedRouteSnapshot,
  //   handle: DetachedRouteHandle,
  // ): void {
  //   console.log("Called CustomRouteReuseStrategy: store");
  //   const path: string = this.getPath(route);
  //   this.storedRoutes.set(path, handle);
  // }

  // override shouldAttach(route: ActivatedRouteSnapshot): boolean {
  //   console.log("Called CustomRouteReuseStrategy: shouldAttach");
  //   const path: string = this.getPath(route);
  //   return !!this.storedRoutes.get(path);
  // }

  // override retrieve(route: ActivatedRouteSnapshot): DetachedRouteHandle | null {
  //   console.log("Called CustomRouteReuseStrategy: retrieve");
  //   const path: string = this.getPath(route);
  //   return this.storedRoutes.get(path) as DetachedRouteHandle;
  // }

  override shouldDetach(route: ActivatedRouteSnapshot): boolean {
    console.log(
      "Called CustomRouteReuseStrategy: shouldDetach: " + route.data.key,
    );
    return route.data.key === "play";
  }

  override store(
    route: ActivatedRouteSnapshot,
    handle: DetachedRouteHandle,
  ): void {
    console.log("Called CustomRouteReuseStrategy: store: " + route.data.key);
    this.storedRoutes.set(route.data.key, handle);
  }

  override shouldAttach(route: ActivatedRouteSnapshot): boolean {
    console.log(
      "Called CustomRouteReuseStrategy: shouldAttach: " + route.data.key,
    );
    return !!this.storedRoutes.get(route.data.key);
  }

  override retrieve(route: ActivatedRouteSnapshot): DetachedRouteHandle | null {
    console.log("Called CustomRouteReuseStrategy: retrieve: " + route.data.key);
    return this.storedRoutes.get(route.data.key) as DetachedRouteHandle;
  }
}
