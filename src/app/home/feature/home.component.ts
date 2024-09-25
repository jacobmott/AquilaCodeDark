import { Component, ViewEncapsulation, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { TopNavLightSliderComponent } from '../../shared/feature/top-nav-light-slider/top-nav-light-slider.component';
import { environment } from '../../../environments/environment';

@Component({
  selector: 'app-aquila-home',
  standalone: true,
  imports: [CommonModule, TopNavLightSliderComponent],
  templateUrl: './home.component.html',
  styleUrl: './home.component.css',
  encapsulation: ViewEncapsulation.Emulated,
})
export class HomeComponent implements OnInit {
  scrollToTop = 0;
  assetsDir: string = environment.assetsDir;
  adventureImgUrl: string = this.assetsDir + '/Adventure.gif';
  ngOnInit(): void {
    this.scrollToTop = 0;
    setInterval(() => {
      this.scrollToTop += 1;
      if (this.scrollToTop >= 175) {
        this.scrollToTop = 0;
      }
    }, 200);
  }
}
