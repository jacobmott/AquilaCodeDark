import { Component, ViewEncapsulation } from "@angular/core";
import { CommonModule } from "@angular/common";
import { Router } from "@angular/router";

@Component({
  selector: "app-aquila-sidebar-button-play",
  standalone: true,
  imports: [CommonModule],
  templateUrl: "./sidebar-button-play.component.html",
  styleUrl: "./sidebar-button-play.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class SidebarButtonPlayComponent {
  constructor(private router: Router) {}

  sSBSFGradientColor = "none";
  sSBSFLightReflection = "none";
  sSBSFTextColor = "#ebebf580";
  sSBSFTextOpacity = "0.5";

  onPointerEnter(event: MouseEvent) {
    this.sSBSFGradientColor = "url(#paint0_radial_11_402)";
    this.sSBSFLightReflection = "url(#paint2_linear_11_402)";
    this.sSBSFTextColor = "#FFFFFF";
    this.sSBSFTextOpacity = "1";
  }
  onPointerLeave(event: MouseEvent) {
    this.sSBSFGradientColor = "none";
    this.sSBSFLightReflection = "none";
    this.sSBSFTextColor = "#ebebf580";
    this.sSBSFTextOpacity = "0.5";
  }
  onPointerDown(event: MouseEvent) {
    console.log("Trying to navigate to play");
    this.router.navigate(["play"]);
  }
}
