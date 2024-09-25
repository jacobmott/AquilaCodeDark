import { Component, ViewEncapsulation } from "@angular/core";
import { CommonModule } from "@angular/common";
import { TopNavLightSliderComponent } from "../../../shared/feature/top-nav-light-slider/top-nav-light-slider.component";
import { ShipTypesComponent } from "../../ui/ship-types/ship-types.component";

@Component({
  selector: "app-aquila-ship-top",
  standalone: true,
  imports: [CommonModule, TopNavLightSliderComponent, ShipTypesComponent],
  templateUrl: "./ship-top.component.html",
  styleUrl: "./ship-top.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class ShipTopComponent {
  scrollToTop = 0;
}
