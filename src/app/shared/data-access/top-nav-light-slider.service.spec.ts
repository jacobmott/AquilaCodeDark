import { TestBed } from '@angular/core/testing';

import { TopNavLightSliderService } from './top-nav-light-slider.service';

describe('TopNavLightSliderService', () => {
  let service: TopNavLightSliderService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(TopNavLightSliderService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
