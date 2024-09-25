import { Component, ViewEncapsulation } from '@angular/core';
import { CommonModule } from '@angular/common';
import { OnInit } from '@angular/core';
import { ShipCardComponent } from '../../ui/ship-card/ship-card.component';
import { TopNavLightSliderComponent } from '../../../shared/feature/top-nav-light-slider/top-nav-light-slider.component';
import { ShipTypesComponent } from '../../ui/ship-types/ship-types.component';

@Component({
  selector: 'app-aquila-ship-list',
  standalone: true,
  imports: [
    CommonModule,
    ShipCardComponent,
    TopNavLightSliderComponent,
    ShipTypesComponent,
  ],
  templateUrl: './ship-list.component.html',
  styleUrl: './ship-list.component.css',
  encapsulation: ViewEncapsulation.Emulated,
})
export class ShipListComponent implements OnInit {
  ships: any[];
  ships2 = '';
  scrollToTop = 0;
  constructor() {
    this.ships = [];
  }

  ngOnInit() {}
}
