import { Component, ViewEncapsulation } from "@angular/core";
import { CommonModule } from "@angular/common";
import { SidebarButtonShipsComponent } from "./sidebar-button-ships.component";
import { SidebarAquilaCodeComponent } from "./sidebar-aquila-code.component";
import { SidebarButtonHomeComponent } from "./sidebar-button-home.component";
import { SidebarButtonPlayComponent } from "./sidebar-button-play.component";

@Component({
  selector: "app-aquila-sidebar",
  standalone: true,
  imports: [
    CommonModule,
    SidebarButtonShipsComponent,
    SidebarAquilaCodeComponent,
    SidebarButtonHomeComponent,
    SidebarButtonPlayComponent,
  ],
  templateUrl: "./sidebar.component.html",
  styleUrl: "./sidebar.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class SidebarComponent {
  aClass =
    "col-span-2 col-start-1 col-end-4 row-start-1 h-4 w-4 rounded-full sm:h-4 sm:w-4 md:h-6 md:w-6 lg:h-10 lg:w-10 xl:h-12 xl:w-12";
  func() {
    return this.aClass;
  }

  arrowFillColor = "#8E37E6";

  onPointerOver(event: MouseEvent) {
    //console.log("onPointerMove");
    this.aClass =
      "bg-aquilapink-600 col-span-2 col-start-1 col-end-4 row-start-1 h-4 w-4 rounded-full sm:h-4 sm:w-4 md:h-6 md:w-6 lg:h-10 lg:w-10 xl:h-12 xl:w-12";
  }
  onPointerOut(event: MouseEvent) {
    //console.log("onPointerMove");
    this.aClass =
      "col-span-2 col-start-1 col-end-4 row-start-1 h-4 w-4 rounded-full sm:h-4 sm:w-4 md:h-6 md:w-6 lg:h-10 lg:w-10 xl:h-12 xl:w-12";
  }
}
