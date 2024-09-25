import { Component, ViewEncapsulation, OnInit } from "@angular/core";
import { CommonModule } from "@angular/common";
import { TopNavLightSliderComponent } from "../../../shared/feature/top-nav-light-slider/top-nav-light-slider.component";

@Component({
  selector: "app-aquila-home-top",
  standalone: true,
  imports: [CommonModule, TopNavLightSliderComponent],
  templateUrl: "./home-top.component.html",
  styleUrl: "./home-top.component.css",
  encapsulation: ViewEncapsulation.Emulated,
})
export class HomeTopComponent implements OnInit {
  scrollToTop = 0;
  ngOnInit(): void {
    this.scrollToTop = 0;
    setInterval(() => {
      this.scrollToTop += 1;
      if (this.scrollToTop >= 175) {
        this.scrollToTop = 0;
      }
      // console.log(this.scrollToTop);
    }, 200);
  }
}
