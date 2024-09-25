import { Route } from "@angular/router";
import { HomeComponent } from "../home.component";
import { HomeTopComponent } from "../home-top/home-top.component";
// import { AuthGuard } from "@auth0/auth0-angular";

export const routes: Route[] = [
  // { path: "", component: HomeComponent, canActivate: [AuthGuard] },
  { path: "", component: HomeComponent },
  // { path: "", component: HomeTopComponent, outlet: "top" },
];
