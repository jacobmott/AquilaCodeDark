import {
  Component,
  ViewEncapsulation,
  ViewChild,
  ElementRef,
  AfterViewInit,
} from "@angular/core";
import { CommonModule } from "@angular/common";
import { TopNavLightSliderService } from "../../data-access/top-nav-light-slider.service";
// import { shareReplay } from "rxjs";

@Component({
  selector: "app-aquila-top-nav-svg",
  standalone: true,
  imports: [CommonModule],
  templateUrl: "./top-nav-light-slider.component.svg",
  styleUrl: "./top-nav-light-slider.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class TopNavLightSliderComponent implements AfterViewInit {
  @ViewChild("mySvg") svgElement!: ElementRef;
  svgRef: SVGSVGElement;

  distanceToCenterOfBlurOnSvg = 53;
  currentXPosition = 0;

  xBoundsLeft = -32;
  xBoundsRight = 1164;

  constructor(private topNavLightSliderService: TopNavLightSliderService) {
    this.currentXPosition =
      this.topNavLightSliderService.getSharedCurrentXPosition();
    // console.log(this.currentXPosition);
    // this.topNavLightSliderService
    //   .getSharedCurrentXPositionBS()
    //   .pipe(shareReplay(1))
    //   .subscribe((position) => {
    //     this.currentXPosition = position;
    //   });
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
