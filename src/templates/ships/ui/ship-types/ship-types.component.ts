import {
  Component,
  ViewEncapsulation,
  ViewChild,
  ElementRef,
  AfterViewInit,
} from "@angular/core";
import { CommonModule } from "@angular/common";
import { Router } from "@angular/router";
import { TopNavLightSliderService } from "../../../shared/top-nav-light-slider/data-access/top-nav-light-slider.service";

@Component({
  selector: "app-aquila-ship-types",
  standalone: true,
  imports: [CommonModule],
  templateUrl: "./ship-types.component.svg",
  styleUrl: "./ship-types.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class ShipTypesComponent implements AfterViewInit {
  @ViewChild("mySvg") svgElement!: ElementRef;
  svgRef: SVGSVGElement;

  distanceToCenterOfBlurOnSvg = 53;
  currentXPosition = 0;

  xBoundsLeft = -32;
  xBoundsRight = 1164;

  constructor(
    private router: Router,
    private topNavLightSliderService: TopNavLightSliderService,
  ) {
    this.currentXPosition =
      this.topNavLightSliderService.getSharedCurrentXPosition();
  }

  // sSBSFGradientColor = "none";
  // sSBSFLightReflection = "none";
  sSBSFTextColorForAll = "#ebebf580";
  sSBSFTextColorForSpeeders = "#ebebf580";
  sSBSFTextColorForBattleships = "#ebebf580";
  sSBSFTextColorForTankers = "#ebebf580";

  onPointerEnterForAll(event: MouseEvent) {
    // this.sSBSFGradientColor = "url(#paint0_radial_11_402)";
    // this.sSBSFLightReflection = "url(#paint2_linear_11_402)";
    this.sSBSFTextColorForAll = "#FFFFFF";
  }
  onPointerLeaveForAll(event: MouseEvent) {
    // this.sSBSFGradientColor = "none";
    // this.sSBSFLightReflection = "none";
    this.sSBSFTextColorForAll = "#ebebf580";
  }
  onPointerDownForAll(event: MouseEvent) {
    console.log("Trying to navigate to all");
    // this.router.navigate(["ships"], { skipLocationChange: true });
  }

  onPointerEnterForSpeeders(event: MouseEvent) {
    // this.sSBSFGradientColor = "url(#paint0_radial_11_402)";
    // this.sSBSFLightReflection = "url(#paint2_linear_11_402)";
    this.sSBSFTextColorForSpeeders = "#FFFFFF";
  }
  onPointerLeaveForSpeeders(event: MouseEvent) {
    // this.sSBSFGradientColor = "none";
    // this.sSBSFLightReflection = "none";
    this.sSBSFTextColorForSpeeders = "#ebebf580";
  }
  onPointerDownForSpeeders(event: MouseEvent) {
    console.log("Trying to navigate to speeders");
    // this.router.navigate(["ships"], { skipLocationChange: true });
  }

  onPointerEnterForBattleships(event: MouseEvent) {
    // this.sSBSFGradientColor = "url(#paint0_radial_11_402)";
    // this.sSBSFLightReflection = "url(#paint2_linear_11_402)";
    this.sSBSFTextColorForBattleships = "#FFFFFF";
  }
  onPointerLeaveForBattleships(event: MouseEvent) {
    // this.sSBSFGradientColor = "none";
    // this.sSBSFLightReflection = "none";
    this.sSBSFTextColorForBattleships = "#ebebf580";
  }
  onPointerDownForBattleships(event: MouseEvent) {
    console.log("Trying to navigate to speeders");
    // this.router.navigate(["ships"], { skipLocationChange: true });
  }

  onPointerEnterForTankers(event: MouseEvent) {
    // this.sSBSFGradientColor = "url(#paint0_radial_11_402)";
    // this.sSBSFLightReflection = "url(#paint2_linear_11_402)";
    this.sSBSFTextColorForTankers = "#FFFFFF";
  }
  onPointerLeaveForTankers(event: MouseEvent) {
    // this.sSBSFGradientColor = "none";
    // this.sSBSFLightReflection = "none";
    this.sSBSFTextColorForTankers = "#ebebf580";
  }
  onPointerDownForTankers(event: MouseEvent) {
    console.log("Trying to navigate to speeders");
    // this.router.navigate(["ships"], { skipLocationChange: true });
  }

  onPointerMove(event: MouseEvent) {
    let point = new DOMPoint(0, 0);
    const { clientX, clientY } = event || { clientX: 0, clientY: 0 };
    point.x = clientX;
    point.y = clientY;
    point = point.matrixTransform(this.svgRef.getScreenCTM()?.inverse());
    // console.dir(point.x);

    const position = point.x - this.distanceToCenterOfBlurOnSvg;
    if (position < this.xBoundsLeft || position > this.xBoundsRight) {
      return;
    }
    this.currentXPosition = position;
    // this.topNavLightSliderService.setSharedCurrentXPositionBS(
    //   this.currentXPosition,
    // );
    this.topNavLightSliderService.setSharedCurrentXPosition(
      this.currentXPosition,
    );
  }
  getTransform() {
    return `translate(${this.currentXPosition})`;
  }

  ngAfterViewInit() {
    const svg = this.svgElement.nativeElement;
    if (svg !== null) {
      this.svgRef = <SVGSVGElement>svg;
    }
  }
}
