import { Routes } from "@angular/router";
import { ShipListComponent } from "../ship-list/ship-list.component";
// import { AuthGuard } from "@auth0/auth0-angular";
// import { ShipTopComponent } from "../ship-top/ship-top.component";

export const routes: Routes = [
  {
    path: "",
    component: ShipListComponent,
  },
  // { path: "", component: ShipTopComponent, outlet: "top" },
];
