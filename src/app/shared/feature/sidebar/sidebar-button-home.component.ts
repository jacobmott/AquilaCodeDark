import { Component, ViewEncapsulation } from "@angular/core";
import { CommonModule } from "@angular/common";
import { Router } from "@angular/router";

@Component({
  selector: "app-aquila-sidebar-button-home",
  standalone: true,
  imports: [CommonModule],
  templateUrl: "./sidebar-button-home.component.svg",
  styleUrl: "./sidebar-button-home.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class SidebarButtonHomeComponent {
  constructor(private router: Router) {}

  sSBSFGradientColor = "none";
  sSBSFLightReflection = "none";
  sSBSFTextColor = "#ebebf580";

  onPointerEnter(event: MouseEvent) {
    this.sSBSFGradientColor = "url(#paint0_radial_11_402)";
    this.sSBSFLightReflection = "url(#paint2_linear_11_402)";
    this.sSBSFTextColor = "#FFFFFF";
  }
  onPointerLeave(event: MouseEvent) {
    this.sSBSFGradientColor = "none";
    this.sSBSFLightReflection = "none";
    this.sSBSFTextColor = "#ebebf580";
  }

  onPointerDown(event: MouseEvent) {
    console.log("Trying to navigate to home");
    this.router.navigate(["home"]);
  }
}
