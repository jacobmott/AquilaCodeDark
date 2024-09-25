import { Component, ViewEncapsulation } from "@angular/core";
import { CommonModule } from "@angular/common";
import { TopNavSvgComponent } from "../../../shared/top-nav-light-slider/ui/top-nav-svg.component";
import { ShipTypesComponent } from "../../ui/ship-types/ship-types.component";

@Component({
  selector: "app-aquila-ship-top",
  standalone: true,
  imports: [CommonModule, TopNavSvgComponent, ShipTypesComponent],
  templateUrl: "./ship-top.component.html",
  styleUrl: "./ship-top.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class ShipTopComponent {
  scrollToTop = 0;
}
