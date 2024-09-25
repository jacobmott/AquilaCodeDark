import { Route } from "@angular/router";
import { PlayComponent } from "../play.component";
// import { AuthGuard } from "@auth0/auth0-angular";

export const routes: Route[] = [
  // { path: "", component: HomeComponent, canActivate: [AuthGuard] },
  { path: "", component: PlayComponent },
];
