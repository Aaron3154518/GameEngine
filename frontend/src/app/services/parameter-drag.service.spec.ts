import { TestBed } from '@angular/core/testing';

import { ParameterDragService } from './parameter-drag.service';

describe('ParameterDragService', () => {
  let service: ParameterDragService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(ParameterDragService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
